/* ScriptData
Name: caio_commandscript
%Complete : 100
Comment : All AIO related server side commands
Category : commandscripts
EndScriptData */

#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "Language.h"

using namespace Acore::ChatCommands;

class caio_commandscript : public CommandScript
{
public:
    caio_commandscript() : CommandScript("caio_commandscript")
    {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable caioCommandTable =
            {
                {"version",        HandleVersionCommand,      SEC_GAMEMASTER, Console::No},
                {"send",           HandleSendCommand,         SEC_GAMEMASTER, Console::No},
                {"forcereload",    HandleReloadCommand,       SEC_GAMEMASTER, Console::No},
                {"forcereset",     HandleResetCommand,        SEC_GAMEMASTER, Console::No},
                {"sendall",        HandleSendAllCommand,      SEC_GAMEMASTER, Console::No},
                {"forcereloadall", HandleReloadAllCommand,    SEC_GAMEMASTER, Console::No},
                {"forceresetall",  HandleResetAllCommand,     SEC_GAMEMASTER, Console::No},
                {"reloadaddons",   HandleReloadAddonsCommand, SEC_GAMEMASTER, Console::No},
                {"addaddon",       HandleAddAddonCommand,     SEC_GAMEMASTER, Console::No},
                {"removeaddon",    HandleRemoveAddonCommand,  SEC_GAMEMASTER, Console::No}
            };

        static ChatCommandTable commandTable =
            {
                {"caio", caioCommandTable}
            };

        return commandTable;
    }

    static bool HandleVersionCommand(ChatHandler *handler)
    {
        handler->PSendSysMessage("AIO version %s.", std::to_string(AIO_VERSION));

        return true;
    }

    static bool HandleSendCommand(ChatHandler *handler, Optional<PlayerIdentifier> player, const std::string &message)
    {
        if (!player) {
            player = PlayerIdentifier::FromTargetOrSelf(handler);
        }

        if (message.empty()) {
            return false;
        }

        Player *target = player->GetConnectedPlayer();

        target->SendSimpleAIOMessage(message);
        handler->PSendSysMessage(LANG_SENDMESSAGE, target->GetName().c_str(), message);

        return true;
    };

    static bool HandleReloadCommand(ChatHandler *handler, Optional<PlayerIdentifier> player)
    {
        if (!player) {
            player = PlayerIdentifier::FromTargetOrSelf(handler);
        }

        Player *target = player->GetConnectedPlayer();

        target->ForceReloadAddons();
        handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, target->GetName().c_str());

        return true;
    };

    static bool HandleResetCommand(ChatHandler *handler, Optional<PlayerIdentifier> player)
    {
        if (!player) {
            player = PlayerIdentifier::FromTargetOrSelf(handler);
        }

        Player *target = player->GetConnectedPlayer();

        target->ForceResetAddons();
        handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, target->GetName().c_str());

        return true;
    };

    static bool HandleSendAllCommand(ChatHandler *handler, const std::string &message)
    {
        if (message.empty()) {
            return false;
        }

        sWorld->SendAllSimpleAIOMessage(message);
        handler->PSendSysMessage(LANG_SENDMESSAGE, "all players", message);

        return true;
    };

    static bool HandleReloadAllCommand(ChatHandler *handler)
    {
        //Force reload required players
        sWorld->ForceReloadPlayerAddons();
        handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, "all players");

        return true;
    };

    static bool HandleResetAllCommand(ChatHandler *handler)
    {
        //Force reset required players
        sWorld->ForceResetPlayerAddons();
        handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, "all players");

        return true;
    };

    static bool HandleReloadAddonsCommand(ChatHandler *handler)
    {
        //Reload and force reload required players
        bool success = sWorld->ReloadAddons();

        if (success) {
            sWorld->ForceReloadPlayerAddons();
        } else {
            handler->SendSysMessage(LANG_CAIO_RELOADADDONS_ERROR);
        }

        return true;
    }

    static bool HandleAddAddonCommand(ChatHandler *handler, const std::string &addon, const std::string &addonFile)
    {
        if (addon.empty() || addonFile.empty()) {
            return false;
        }

        AIOAddon newAddon(addon, addonFile);

        bool added = sWorld->AddAddon(newAddon);

        if (added) {
            sWorld->ForceReloadPlayerAddons();
        } else {
            handler->PSendSysMessage(LANG_CAIO_ADDADDON_ERROR, addon);
        }

        return true;
    }

    static bool HandleRemoveAddonCommand(ChatHandler *handler, const std::string &addon)
    {
        if (addon.empty()) {
            return false;
        }

        //Remove and reload required players
        if (sWorld->RemoveAddon(addon)) {
            sWorld->ForceReloadPlayerAddons();
        } else {
            handler->PSendSysMessage(LANG_CAIO_REMOVEADDON_ERROR, addon);
        }

        return true;
    }
};

void AddSC_caio_commandscript()
{
    new caio_commandscript();
}
