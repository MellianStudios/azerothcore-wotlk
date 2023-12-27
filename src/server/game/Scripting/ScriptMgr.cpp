/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "AllScriptsObjects.h"
#include "LFGScripts.h"
#include "InstanceScript.h"
#include "ScriptObject.h"
#include "ScriptSystem.h"
#include "SmartAI.h"
#include "SpellMgr.h"
#include "UnitAI.h"
#include "smallfolk.h"

namespace
{
    template<typename T>
    inline void SCR_CLEAR()
    {
        for (auto const& [scriptID, script] : ScriptRegistry<T>::ScriptPointerList)
        {
            delete script;
        }

        ScriptRegistry<T>::ScriptPointerList.clear();
    }
}

struct TSpellSummary
{
    uint8 Targets; // set of enum SelectTarget
    uint8 Effects; // set of enum SelectEffect
}*SpellSummary;

ScriptMgr::ScriptMgr()
    : _scriptCount(0),
      _aioHandlers(nullptr),
      _scheduledScripts(0),
      _script_loader_callback(nullptr),
      _modules_loader_callback(nullptr)
{}

ScriptMgr::~ScriptMgr() { }

ScriptMgr* ScriptMgr::instance()
{
    static ScriptMgr instance;
    return &instance;
}

void ScriptMgr::Initialize()
{
    LOG_INFO("server.loading", "> Loading C++ scripts");
    LOG_INFO("server.loading", " ");

    _aioHandlers = new AIOHandlers();

    AddSC_SmartScripts();

    // LFGScripts
    lfg::AddSC_LFGScripts();

    ASSERT(_script_loader_callback,
        "Script loader callback wasn't registered!");

    ASSERT(_modules_loader_callback,
        "Modules loader callback wasn't registered!");

    _script_loader_callback();
    _modules_loader_callback();
}

void ScriptMgr::Unload()
{
    SCR_CLEAR<AccountScript>();
    SCR_CLEAR<AchievementCriteriaScript>();
    SCR_CLEAR<AchievementScript>();
    SCR_CLEAR<AllCreatureScript>();
    SCR_CLEAR<AllGameObjectScript>();
    SCR_CLEAR<AllItemScript>();
    SCR_CLEAR<AllMapScript>();
    SCR_CLEAR<AreaTriggerScript>();
    SCR_CLEAR<ArenaScript>();
    SCR_CLEAR<ArenaTeamScript>();
    SCR_CLEAR<AuctionHouseScript>();
    SCR_CLEAR<BGScript>();
    SCR_CLEAR<BattlegroundMapScript>();
    SCR_CLEAR<BattlegroundScript>();
    SCR_CLEAR<CommandSC>();
    SCR_CLEAR<CommandScript>();
    SCR_CLEAR<ConditionScript>();
    SCR_CLEAR<CreatureScript>();
    SCR_CLEAR<DatabaseScript>();
    SCR_CLEAR<DynamicObjectScript>();
    SCR_CLEAR<ElunaScript>();
    SCR_CLEAR<FormulaScript>();
    SCR_CLEAR<GameEventScript>();
    SCR_CLEAR<GameObjectScript>();
    SCR_CLEAR<GlobalScript>();
    SCR_CLEAR<GroupScript>();
    SCR_CLEAR<GuildScript>();
    SCR_CLEAR<InstanceMapScript>();
    SCR_CLEAR<ItemScript>();
    SCR_CLEAR<LootScript>();
    SCR_CLEAR<MailScript>();
    SCR_CLEAR<MiscScript>();
    SCR_CLEAR<MovementHandlerScript>();
    SCR_CLEAR<OutdoorPvPScript>();
    SCR_CLEAR<PetScript>();
    SCR_CLEAR<PlayerScript>();
    SCR_CLEAR<ServerScript>();
    SCR_CLEAR<SpellSC>();
    SCR_CLEAR<SpellScriptLoader>();
    SCR_CLEAR<TransportScript>();
    SCR_CLEAR<UnitScript>();
    SCR_CLEAR<VehicleScript>();
    SCR_CLEAR<WeatherScript>();
    SCR_CLEAR<WorldMapScript>();
    SCR_CLEAR<WorldObjectScript>();
    SCR_CLEAR<WorldScript>();
    SCR_CLEAR<AIOScript>();

    delete[] SpellSummary;
}

void ScriptMgr::LoadDatabase()
{
    uint32 oldMSTime = getMSTime();

    sScriptSystemMgr->LoadScriptWaypoints();

    // Add all scripts that must be loaded after db/maps
    ScriptRegistry<WorldMapScript>::AddALScripts();
    ScriptRegistry<BattlegroundMapScript>::AddALScripts();
    ScriptRegistry<InstanceMapScript>::AddALScripts();
    ScriptRegistry<SpellScriptLoader>::AddALScripts();
    ScriptRegistry<ItemScript>::AddALScripts();
    ScriptRegistry<CreatureScript>::AddALScripts();
    ScriptRegistry<GameObjectScript>::AddALScripts();
    ScriptRegistry<AreaTriggerScript>::AddALScripts();
    ScriptRegistry<BattlegroundScript>::AddALScripts();
    ScriptRegistry<OutdoorPvPScript>::AddALScripts();
    ScriptRegistry<WeatherScript>::AddALScripts();
    ScriptRegistry<ConditionScript>::AddALScripts();
    ScriptRegistry<TransportScript>::AddALScripts();
    ScriptRegistry<AchievementCriteriaScript>::AddALScripts();

    FillSpellSummary();

    CheckIfScriptsInDatabaseExist();

    LOG_INFO("server.loading", ">> Loaded {} C++ scripts in {} ms", GetScriptCount(), GetMSTimeDiffToNow(oldMSTime));
    LOG_INFO("server.loading", " ");
}

void ScriptMgr::CheckIfScriptsInDatabaseExist()
{
    for (auto const& scriptName : sObjectMgr->GetScriptNames())
    {
        if (uint32 sid = sObjectMgr->GetScriptId(scriptName))
        {
            if (!ScriptRegistry<SpellScriptLoader>::GetScriptById(sid) &&
                !ScriptRegistry<ServerScript>::GetScriptById(sid) &&
                !ScriptRegistry<WorldScript>::GetScriptById(sid) &&
                !ScriptRegistry<FormulaScript>::GetScriptById(sid) &&
                !ScriptRegistry<WorldMapScript>::GetScriptById(sid) &&
                !ScriptRegistry<InstanceMapScript>::GetScriptById(sid) &&
                !ScriptRegistry<BattlegroundMapScript>::GetScriptById(sid) &&
                !ScriptRegistry<ItemScript>::GetScriptById(sid) &&
                !ScriptRegistry<CreatureScript>::GetScriptById(sid) &&
                !ScriptRegistry<GameObjectScript>::GetScriptById(sid) &&
                !ScriptRegistry<AreaTriggerScript>::GetScriptById(sid) &&
                !ScriptRegistry<BattlegroundScript>::GetScriptById(sid) &&
                !ScriptRegistry<OutdoorPvPScript>::GetScriptById(sid) &&
                !ScriptRegistry<CommandScript>::GetScriptById(sid) &&
                !ScriptRegistry<WeatherScript>::GetScriptById(sid) &&
                !ScriptRegistry<AuctionHouseScript>::GetScriptById(sid) &&
                !ScriptRegistry<ConditionScript>::GetScriptById(sid) &&
                !ScriptRegistry<VehicleScript>::GetScriptById(sid) &&
                !ScriptRegistry<DynamicObjectScript>::GetScriptById(sid) &&
                !ScriptRegistry<TransportScript>::GetScriptById(sid) &&
                !ScriptRegistry<AchievementCriteriaScript>::GetScriptById(sid) &&
                !ScriptRegistry<PlayerScript>::GetScriptById(sid) &&
                !ScriptRegistry<GuildScript>::GetScriptById(sid) &&
                !ScriptRegistry<BGScript>::GetScriptById(sid) &&
                !ScriptRegistry<AchievementScript>::GetScriptById(sid) &&
                !ScriptRegistry<ArenaTeamScript>::GetScriptById(sid) &&
                !ScriptRegistry<SpellSC>::GetScriptById(sid) &&
                !ScriptRegistry<MiscScript>::GetScriptById(sid) &&
                !ScriptRegistry<PetScript>::GetScriptById(sid) &&
                !ScriptRegistry<CommandSC>::GetScriptById(sid) &&
                !ScriptRegistry<ArenaScript>::GetScriptById(sid) &&
                !ScriptRegistry<GroupScript>::GetScriptById(sid) &&
                !ScriptRegistry<DatabaseScript>::GetScriptById(sid))
                {
                    LOG_ERROR("sql.sql", "Script named '{}' is assigned in the database, but has no code!", scriptName);
                }
        }
    }
}

void ScriptMgr::FillSpellSummary()
{
    UnitAI::FillAISpellInfo();

    SpellSummary = new TSpellSummary[sSpellMgr->GetSpellInfoStoreSize()];

    SpellInfo const* pTempSpell;

    for (uint32 i = 0; i < sSpellMgr->GetSpellInfoStoreSize(); ++i)
    {
        SpellSummary[i].Effects = 0;
        SpellSummary[i].Targets = 0;

        pTempSpell = sSpellMgr->GetSpellInfo(i);
        // This spell doesn't exist.
        if (!pTempSpell)
            continue;

        for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            // Spell targets self.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SELF - 1);

            // Spell targets a single enemy.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_TARGET_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_ENEMY - 1);

            // Spell targets AoE at enemy.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_SRC_AREA_ENEMY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_ENEMY - 1);

            // Spell targets an enemy.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_TARGET_ENEMY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_SRC_AREA_ENEMY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_ENEMY - 1);

            // Spell targets a single friend (or self).
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_PARTY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_FRIEND - 1);

            // Spell targets AoE friends.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER_AREA_PARTY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_LASTTARGET_AREA_PARTY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_FRIEND - 1);

            // Spell targets any friend (or self).
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_PARTY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER_AREA_PARTY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_LASTTARGET_AREA_PARTY ||
                    pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_FRIEND - 1);

            // Make sure that this spell includes a damage effect.
            if (pTempSpell->Effects[j].Effect == SPELL_EFFECT_SCHOOL_DAMAGE ||
                    pTempSpell->Effects[j].Effect == SPELL_EFFECT_INSTAKILL ||
                    pTempSpell->Effects[j].Effect == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE ||
                    pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEALTH_LEECH)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_DAMAGE - 1);

            // Make sure that this spell includes a healing effect (or an apply aura with a periodic heal).
            if (pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEAL ||
                    pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                    pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEAL_MECHANICAL ||
                    (pTempSpell->Effects[j].Effect == SPELL_EFFECT_APPLY_AURA  && pTempSpell->Effects[j].ApplyAuraName == 8))
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_HEALING - 1);

            // Make sure that this spell applies an aura.
            if (pTempSpell->Effects[j].Effect == SPELL_EFFECT_APPLY_AURA)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_AURA - 1);
        }
    }
}

AIOScript::AIOScriptByKeyMap AIOScript::_scriptByKeyMap = AIOScript::AIOScriptByKeyMap();

void ScriptMgr::OnAddonMessage(Player *sender, const std::string &message)
{
    if (!sender) {
        return;
    }

    LuaVal mainTable = LuaVal::loads(message);

    if (!mainTable.istable()) { //Unable to parse or incorrect format
        return;
    }

    //Call handlers from all blocks in order
    for (size_t i = 1; i <= mainTable.tbl().size(); ++i) {
        LuaVal &block = mainTable[1];

        if (!block.istable()) {
            continue;
        }

        LuaVal &scriptKeyVal = block[2];
        LuaVal &handlerKeyVal = block[3];

        if (!block[1].isnumber() || scriptKeyVal.isnil() || handlerKeyVal.isnil()) {
            continue;
        }

        if (AIOScript *aioScript = _aioHandlers->GetScript<AIOScript>(scriptKeyVal)) {
            aioScript->OnHandle(sender, handlerKeyVal, block);
        }
    }
}

AIOScript::AIOScript(const LuaVal &scriptKey) : ScriptObject(scriptKey.tostring().c_str()), _key(scriptKey)
{
    if (AIOScript::_scriptByKeyMap.find(scriptKey) != AIOScript::_scriptByKeyMap.end()) {
        sLog->outAIOMessage(
            0,
            LOG_LEVEL_FATAL,
            "AIO scriptKey '%s' of type tag '%i' already exist. Use another key.",
            scriptKey.tostring().c_str(),
            scriptKey.typetag()
        );

            ASSERT(false);
    }

    ScriptRegistry<AIOScript>::AddScript(this);
    AIOScript::_scriptByKeyMap[scriptKey] = this;
}

void AIOScript::AddInitArgs(
    const LuaVal &scriptKey,
    const LuaVal &handlerKey,
    ArgFunc a1,
    ArgFunc a2,
    ArgFunc a3,
    ArgFunc a4,
    ArgFunc a5,
    ArgFunc a6
)
{
    AIOHandlers *handler = sScriptMgr->_aioHandlers;

    if (!handler) {
        return;
    }

    //Look for hook
    std::list<ArgFunc> *list = 0;

    for (auto &itr: handler->_initHookList) {
        if (itr.scriptKey == scriptKey && itr.handlerKey == handlerKey) {
            list = &itr.argsList;
            break;

        }
    }

    //Add hook
    if (!list) {
        handler->_initHookList.push_back(AIOHandlers::InitHookInfo(scriptKey, handlerKey));
        list = &handler->_initHookList.back().argsList;
    }

    //Add args
    if (a1) {
        list->push_back(a1);
    }

    if (a2) {
        list->push_back(a2);
    }

    if (a3) {
        list->push_back(a3);
    }

    if (a4) {
        list->push_back(a4);
    }

    if (a5) {
        list->push_back(a5);
    }

    if (a6) {
        list->push_back(a6);
    }
}

template<>
AIOScript *AIOScript::GetScript(const LuaVal &scriptKey)
{
    AIOScriptByKeyMap::const_iterator itr = AIOScript::_scriptByKeyMap.find(scriptKey);

    if (itr == AIOScript::_scriptByKeyMap.end()) {
        return 0;
    }

    return itr->second;
}

template<class ScriptClass>
ScriptClass *AIOScript::GetScript(const LuaVal &scriptKey)
{
    AIOScriptByKeyMap::const_iterator itr = AIOScript::_scriptByKeyMap.find(scriptKey);

    if (itr == AIOScript::_scriptByKeyMap.end()) {
        return 0;
    }

    return dynamic_cast<ScriptClass *>(itr->second);
}

void AIOScript::OnHandle(Player *sender, const LuaVal &handlerKey, const LuaVal &args)
{
    HandlerMapType::const_iterator itr = _handlerMap.find(handlerKey);

    if (itr != _handlerMap.end()) {
        itr->second(sender, args); //Call the handler function
    }
}

AIOHandlers::AIOHandlers() : AIOScript("AIO")
{
    AddHandler("Init", std::bind(&AIOHandlers::HandleInit, this, std::placeholders::_1, std::placeholders::_2));
    AddHandler("Error", std::bind(&AIOHandlers::HandleError, this, std::placeholders::_1, std::placeholders::_2));
}

void AIOHandlers::HandleInit(Player *sender, const LuaVal &args)
{
    //Init hasn't cooled down
    if (sender->isAIOInitOnCooldown()) {
        return;
    }

    sender->setAIOIntOnCooldown(true);
    const LuaVal &versionVal = args[4];
    const LuaVal &clientDataVal = args[5];

    if (!versionVal.isnumber() || !clientDataVal.istable()) {
        sLog->outAIOMessage(
            sender->GetGUID().GetCounter(),
            LOG_LEVEL_ERROR,
            "AIOHandlers::HandleInit: Invalid version value or clientData value. Sender: %s, Args: %s",
            sender->GetName().c_str(),
            args.dumps().c_str()
        );
        return;
    }

    if (versionVal.num() != AIO_VERSION) {
        sender->AIOHandle("AIO", "Init", AIO_VERSION);

        return;
    }

    LuaVal addonTable(TTABLE);
    LuaVal cacheTable(TTABLE);
    uint32 nAddons = sWorld->PrepareClientAddons(clientDataVal, addonTable, cacheTable, sender);

    LuaVal argsToSend(TTABLE);

    uint32 blockIndex = 1;

    for (HookListType::const_iterator itr = _initHookList.begin(); itr != _initHookList.end(); ++itr) {
        uint32 index = 3;
        LuaVal HookBlock(TTABLE);

        HookBlock[1] = (uint32) itr->argsList.size() + 1;
        HookBlock[2] = itr->scriptKey;
        HookBlock[3] = itr->handlerKey;

        for (const auto &it: itr->argsList) {
            HookBlock[++index] = it(sender);
        }

        argsToSend[++blockIndex] = HookBlock;
    }

    LuaVal AIOInitBlock(TTABLE);
    AIOInitBlock[1] = 5;
    AIOInitBlock[2] = "AIO";
    AIOInitBlock[3] = "Init";
    AIOInitBlock[4] = AIO_VERSION;
    AIOInitBlock[5] = nAddons;
    AIOInitBlock[6] = addonTable;
    AIOInitBlock[7] = cacheTable;

    argsToSend[1] = AIOInitBlock;
    sender->SendSimpleAIOMessage(argsToSend.dumps());

    sender->m_aioInitialized = true;
}

void AIOHandlers::HandleError(Player *sender, const LuaVal &args)
{
    const LuaVal &msgVal = args[4];

    if (!msgVal.isstring()) {
        return;
    }

    sLog->outAIOMessage(
        sender->GetGUID().GetCounter(),
        LOG_LEVEL_ERROR,
        "%s Received client addon error: %s",
        sender->GetSession()->GetPlayerInfo().c_str(),
        msgVal.str().c_str()
    );
}
