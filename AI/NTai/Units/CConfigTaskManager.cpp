
#include "../Core/include.h"

namespace ntai {
	

	CConfigTaskManager::CConfigTaskManager(Global* G, int unit){
		//
		this->G = G;
		nolist = false;
		repeat=true;
		this->unit = unit;
		LoadTaskList();
		
	}

	bool CConfigTaskManager::Init(){
		//
		return true;
	}

	void CConfigTaskManager::RecieveMessage(CMessage &message){
		//
	}

	void CConfigTaskManager::TaskFinished(){
		//
		EraseFirst();
		if(!HasTasks()){
			LoadTaskList();
		}
	}
	
	bool CConfigTaskManager::HasTasks(){
		//
		return !tasks.empty();
	}
	
	IModule* CConfigTaskManager::GetNextTask(){
		//
		if(HasTasks()){
			return tasks.front();
		} else {
			return 0;
		}
	}
	
	void CConfigTaskManager::RemoveAllTasks(){
		//
		if(HasTasks()){
			tasks.erase(tasks.begin(),tasks.end());
		}
	}

	bool CConfigTaskManager::LoadTaskList(){
		NLOG("CConfigTaskManager::LoadTaskList");

		CUnit* u = G->unit_array[unit];

		CUnitTypeData* utd = u->GetUnitDataType();

		std::vector<std::string> vl;
		std::string listname = utd->GetName();
		if(G->info->_abstract){
			// default configuration file!!!!!
			if(utd->IsFactory()){
				listname = "factory";
			} else if (utd->IsHub()){
				listname = "hub";
			} else if (utd->CanConstruct()){
				//
				if(utd->GetUnitDef()->isCommander){
					listname = "commander";
				} else {
					listname = "builder";
				}
			}
		}
		std::string sl = "";
		if(G->Cached->cheating){
			G->Get_mod_tdf()->SGetValue(sl, std::string("TASKLISTS\\CHEAT\\")+listname);
		} else {
			G->Get_mod_tdf()->SGetValue(sl, std::string("TASKLISTS\\NORMAL\\")+listname);
		}

		tolowercase(sl);
		trim(sl);
		std::string us = utd->GetName();
		if(sl != std::string("")){
			CTokenizer<CIsComma>::Tokenize(vl, sl, CIsComma());
			if(vl.empty() == false){
				int randnum = G->mrand()%vl.size();
				us = vl.at(std::min(randnum,std::max(int(vl.size()-1),1)));
			}
		}

		std::string s = "";
		G->Get_mod_tdf()->SGetValue(s,std::string("TASKLISTS\\LISTS\\")+us);

		if(s.empty()){
			G->L.print(" error loading tasklist for unit :: \"" + us + "\" :: buffer empty, most likely because of an empty list");
			nolist=true;
			return false;
		}

		tolowercase(s);
		trim(s);

		std::vector<std::string> v;

		CTokenizer<CIsComma>::Tokenize(v, s, CIsComma());

		if(v.empty() == false){
			G->L.print("loading contents of  tasklist :: " + us + " :: filling tasklist with #" + to_string(v.size()) + " items");
			bool polate=false;
			bool polation = G->info->rule_extreme_interpolate;
			btype bt = G->Manufacturer->GetTaskType(G->Get_mod_tdf()->SGetValueDef("b_na","AI\\interpolate_tag"));

			if(utd->IsFactory()){
				polation = false;
			}

			if(bt == B_NA){
				polation = false;
			}


			// TASKS LOADING
			
			for(std::vector<std::string>::iterator vi = v.begin(); vi != v.end(); ++vi){
				if(polation){
					if(polate){
						IModule* t = new CKeywordConstructionTask(G,u->GetID(),bt);
						tasks.push_back(t);
					}
					polate = !polate;
				}
				std::string q = *vi;

				trim(q);
				tolowercase(q);

				CUnitTypeData* b = G->UnitDefLoader->GetUnitTypeDataByName(q);
				if(b != 0){
					if(u->GetUnitDataType()->CanBuild(q)){
						IModule* t = new CUnitConstructionTask(G,u->GetID(),utd,b);
						tasks.push_back(t);
					} else {
						continue;
					}
				}else if(q == std::string("")){
					continue;
				}else if(q == std::string("b_na")){
					continue;
				} else if(q == std::string("no_rule_interpolation")){
					polation=false;
				} else if(q == std::string("rule_interpolate")){
					polation=true;
				}else if(q == std::string("base_pos")){
					G->Map->base_positions.push_back(G->GetUnitPos(u->GetID()));
				} else if(q == std::string("gaia")){
					G->info->gaia = true;
				} else if(q == std::string("not_gaia")){
					G->info->gaia = false;
				} else if(q == std::string("switch_gaia")){
					G->info->gaia = !G->info->gaia;
				} else if(q == std::string("b_factory")){
					IModule* t = new CKeywordConstructionTask(G,u->GetID(),B_FACTORY);
					tasks.push_back(t);
				} else if(q == std::string("b_power")){
					IModule* t = new CKeywordConstructionTask(G,u->GetID(),B_POWER);
					tasks.push_back(t);
				} else if(q == std::string("b_defence")){
					IModule* t = new CKeywordConstructionTask(G,u->GetID(),B_DEFENCE);
					tasks.push_back(t);
				}  else if(q == std::string("b_mex")){
					IModule* t = new CKeywordConstructionTask(G,u->GetID(),B_MEX);
					tasks.push_back(t);
				} else{
					btype x = G->Manufacturer->GetTaskType(q);
					if( x != B_NA){
						IModule* t = new CKeywordConstructionTask(G,u->GetID(),x);
						tasks.push_back(t);
					}else{
						G->L.print("error :: a value :: " + *vi +" :: was parsed in :: "+us + " :: this does not have a valid UnitDef according to the engine, and is not a Task keyword such as repair or b_mex");
					}
				}

			}
			if(utd->GetUnitDef()->isCommander){
				G->Map->basepos = G->GetUnitPos(u->GetID());
			}

			G->L.print("loaded contents of  tasklist :: " + us + " :: loaded tasklist at " + to_string(tasks.size()) + " items");
			return !tasks.empty();
		} else{
			G->L.print(" error loading contents of  tasklist :: " + us + " :: buffer empty, most likely because of an empty tasklist");
			return false;
		}
	}
	
	bool CConfigTaskManager::EraseFirst(){
		if(!tasks.empty()){
			tasks.erase(tasks.begin());
			return true;
		}
		return false;
	}

}
