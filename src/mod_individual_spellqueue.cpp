/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Chat.h"
#include "Config.h"
#include "DataMap.h"
#include "Object.h"
#include "Player.h"
#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

constexpr const char* MODULE_STRING = "individual-spellqueue";

struct IndividualSpellQueueModule
{
    bool Enabled;
    uint32 Default;
    uint32 MinWindow;
    uint32 MaxWindow;
    bool LatencyDefault;
};

enum IndividualSpellQueueStrings
{
    INFO_INDIVIDUAL_SPELLQUEUE_VIEW_WINDOW = 1,
    ERROR_INDIVIDUAL_SPELLQUEUE_MAX_WINDOW = 2,
    ERROR_INDIVIDUAL_SPELLQUEUE_MIN_WINDOW = 3,
    SUCCESS_INDIVIDUAL_SPELLQUEUE_UPDATED = 4,
    SUCCESS_INDIVIDUAL_SPELLQUEUE_DISABLED = 5,
    SUCCESS_INDIVIDUAL_SPELLQUEUE_ENABLED = 6,
    SUCCESS_INDIVIDUAL_SPELLQUEUE_DEFAULT = 7,
    ERROR_INDIVIDUAL_SPELLQUEUE_MODULE_DISABLED = 8,
    SUCCESS_INDIVIDUAL_SPELLQUEUE_LATENCY_ENABLED = 9,
    SUCCESS_INDIVIDUAL_SPELLQUEUE_LATENCY_DISABLED = 10
};

IndividualSpellQueueModule individualSpellQueue;

class PlayerIndividualSpellQueueWindow : public DataMap::Base
{
public:
    PlayerIndividualSpellQueueWindow() : window(individualSpellQueue.Default), latency(individualSpellQueue.LatencyDefault) {}
    PlayerIndividualSpellQueueWindow(uint32 window, bool latency) : window(window), latency(latency) {}
    uint32 window;
    bool latency;
};

class IndividualSpellQueueConfig : public WorldScript
{
public:
    IndividualSpellQueueConfig() : WorldScript("IndividualSpellQueueConfig") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        individualSpellQueue.Enabled = sConfigMgr->GetOption<bool>("IndividualSpellQueue.Enabled", true);
        individualSpellQueue.Default = sConfigMgr->GetOption<uint32>("IndividualSpellQueue.Default", 400);
        individualSpellQueue.MinWindow = sConfigMgr->GetOption<uint32>("IndividualSpellQueue.MinWindow", 100);
        individualSpellQueue.MaxWindow = sConfigMgr->GetOption<uint32>("IndividualSpellQueue.MaxWindow", 600);
        individualSpellQueue.LatencyDefault = sConfigMgr->GetOption<bool>("IndividualSpellQueue.LatencyDefault", false);

        if (individualSpellQueue.Enabled && !sConfigMgr->GetOption<bool>("SpellQueue.Enabled", true))
            LOG_ERROR("module", "IndividualSpellQueue::OnAfterConfigLoad Module is enabled, but SpellQueue is disabled!. Please adjust your worldserver.conf or individual_spellqueue.conf");
    }
};

class IndividualSpellQueue : public PlayerScript
{
public:
    IndividualSpellQueue() : PlayerScript("PlayerIndividualSpellQueue") { }

    void OnLogin(Player* player) override
    {
        if (!individualSpellQueue.Enabled)
            return;

        if (QueryResult result = CharacterDatabase.Query("SELECT `window`, `latency` FROM `mod_individual_spellqueue` WHERE `guid`='{}'", player->GetGUID().GetCounter()))
        {
            Field* fields = result->Fetch();
            player->CustomData.Set("IndividualSpellQueue", new PlayerIndividualSpellQueueWindow(fields[0].Get<uint32>(),(fields[1].Get<bool>())));
        }
    }

    void OnLogout(Player* player) override
    {
        if (PlayerIndividualSpellQueueWindow* data = player->CustomData.Get<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue"))
            CharacterDatabase.DirectExecute("REPLACE INTO `mod_individual_spellqueue` (`guid`, `window`, `latency`) VALUES ('{}', '{}', '{}');", player->GetGUID().GetCounter(), data->window, data->latency);
    }

   void OnGetSpellQueueWindow(const Player* player, uint32& window) override
    {
        if (!individualSpellQueue.Enabled)
            return;

        if (PlayerIndividualSpellQueueWindow* data = player->CustomData.Get<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue"))
        {
            window = data->window;
            // If window is enabled (non-0) and latency compensation is enabled
            if (window && data->latency)
                window += player->GetSession()->GetLatency();
            window = std::min(window, individualSpellQueue.MaxWindow);
        }
   }
};

class IndividualSpellQueueCommand : public CommandScript
{
public:
    IndividualSpellQueueCommand() : CommandScript("IndividualSpellQueueCommand") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable IndividualSpellQueueCommandTable =
        {
            { "default",  HandleDefaultCommand, SEC_PLAYER, Console::No },
            { "disable",  HandleDisableCommand, SEC_PLAYER, Console::No },
            { "enable",  HandleEnableCommand, SEC_PLAYER, Console::No },
            { "latency",  HandleLatencyCommand, SEC_PLAYER, Console::No },
            { "set",  HandleSetCommand, SEC_PLAYER, Console::No },
            { "view",  HandleViewCommand, SEC_PLAYER, Console::No }
        };

        static ChatCommandTable IndividualSpellQueueBaseTable =
        {
            { "spellqueue",  IndividualSpellQueueCommandTable }
        };

        return IndividualSpellQueueBaseTable;
    }

    static bool HandleDefaultCommand(ChatHandler* handler)
    {
        if (!individualSpellQueue.Enabled)
        {
            handler->PSendModuleSysMessage(MODULE_STRING, ERROR_INDIVIDUAL_SPELLQUEUE_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        player->CustomData.GetDefault<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue")->window = individualSpellQueue.Default;;
        handler->PSendModuleSysMessage(MODULE_STRING, SUCCESS_INDIVIDUAL_SPELLQUEUE_DEFAULT, individualSpellQueue.Default);
        return true;
    }

    static bool HandleDisableCommand(ChatHandler* handler)
    {
        if (!individualSpellQueue.Enabled)
        {
            handler->PSendModuleSysMessage(MODULE_STRING, ERROR_INDIVIDUAL_SPELLQUEUE_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        player->CustomData.GetDefault<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue")->window = 0;
        handler->PSendModuleSysMessage(MODULE_STRING, SUCCESS_INDIVIDUAL_SPELLQUEUE_DISABLED);
        return true;
    }

    static bool HandleEnableCommand(ChatHandler* handler)
    {
        if (!individualSpellQueue.Enabled)
        {
            handler->PSendModuleSysMessage(MODULE_STRING, ERROR_INDIVIDUAL_SPELLQUEUE_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        player->CustomData.GetDefault<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue")->window = individualSpellQueue.Default;;
        handler->PSendModuleSysMessage(MODULE_STRING, SUCCESS_INDIVIDUAL_SPELLQUEUE_ENABLED, individualSpellQueue.Default);
        return true;
    }

    static bool HandleLatencyCommand(ChatHandler* handler)
    {
        if (!individualSpellQueue.Enabled)
        {
            handler->PSendModuleSysMessage(MODULE_STRING, ERROR_INDIVIDUAL_SPELLQUEUE_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        // toggle latency compensation
        bool latency = player->CustomData.GetDefault<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue")->latency;
        player->CustomData.GetDefault<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue")->latency = !latency;

        handler->PSendModuleSysMessage(MODULE_STRING,
            player->CustomData.GetDefault<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue")->latency
                ? SUCCESS_INDIVIDUAL_SPELLQUEUE_LATENCY_ENABLED : SUCCESS_INDIVIDUAL_SPELLQUEUE_LATENCY_DISABLED,
            player->CustomData.GetDefault<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue")->window
        );
        return true;
    }

    static bool HandleSetCommand(ChatHandler* handler, uint32 window)
    {
        if (!individualSpellQueue.Enabled)
        {
            handler->PSendModuleSysMessage(MODULE_STRING, ERROR_INDIVIDUAL_SPELLQUEUE_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!window)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        if (window < individualSpellQueue.MinWindow)
        {
            handler->PSendModuleSysMessage(MODULE_STRING, ERROR_INDIVIDUAL_SPELLQUEUE_MIN_WINDOW, individualSpellQueue.MinWindow);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (window > individualSpellQueue.MaxWindow)
        {
            handler->PSendModuleSysMessage(MODULE_STRING, ERROR_INDIVIDUAL_SPELLQUEUE_MAX_WINDOW, individualSpellQueue.MaxWindow);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->CustomData.GetDefault<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue")->window = window;
        handler->PSendModuleSysMessage(MODULE_STRING, SUCCESS_INDIVIDUAL_SPELLQUEUE_UPDATED, window);
        return true;
    }

    static bool HandleViewCommand(ChatHandler* handler)
    {
        if (!individualSpellQueue.Enabled)
        {
            handler->PSendModuleSysMessage(MODULE_STRING, ERROR_INDIVIDUAL_SPELLQUEUE_MODULE_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        uint32 window = player->CustomData.GetDefault<PlayerIndividualSpellQueueWindow>("IndividualSpellQueue")->window;
        handler->PSendModuleSysMessage(MODULE_STRING, INFO_INDIVIDUAL_SPELLQUEUE_VIEW_WINDOW, window);
        return true;
    }
};

void AddSC_individual_spellqueue_script()
{
    new IndividualSpellQueueConfig();
    new IndividualSpellQueue();
    new IndividualSpellQueueCommand();
}