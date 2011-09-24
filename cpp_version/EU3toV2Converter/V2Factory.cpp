#include "V2Factory.h"
#include "Parsers/Parser.h"
#include "Log.h"


void V2FactoryType::init(Object* factory)
{
	name = factory->getKey();

	requireInvention = (inventionType)-1;

	vector<Object*> goods = factory->getValue("input_goods");
	if (goods.size() > 0)
	{
		vector<Object*> inObjs = goods[0]->getLeaves();
		for (vector<Object*>::iterator itr = inObjs.begin(); itr != inObjs.end(); ++itr)
		{
			inputs.insert(make_pair((*itr)->getKey(), (float)atof((*itr)->getLeaf().c_str())));
		}
	}

	vector<Object*> local_supply = factory->getValue("limit_by_local_supply");
	if ((local_supply.size() > 0) && (local_supply[0]->getLeaf() == "yes"))
	{
		requireLocalInput = true;
	}
	else
	{
		requireLocalInput = false;
	}

	vector<Object*> is_coastal = factory->getValue("is_coastal");
	if ((is_coastal.size() > 0) && (is_coastal[0]->getLeaf() == "yes"))
	{
		requireCoastal = true;
	}
	else
	{
		requireCoastal = false;
	}
}


vector<string> V2Factory::getRequiredRGO() const
{
	vector<string> retval;
	if (type->requireLocalInput)
	{
		for (map<string,float>::const_iterator itr = type->inputs.begin(); itr != type->inputs.end(); ++itr)
		{
			retval.push_back(itr->first);
		}
	}
	return retval;
}


void V2Factory::output(FILE* output)
{
	// V2 takes care of hiring employees on day 1, provided sufficient starting capital

	fprintf(output, "\t\tstate_buildings=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\tbuilding=\"%s\"\n", type->name.c_str());
	fprintf(output, "\t\t\tlevel=1\n");

	// stockpile 1/2 of necessary input (seems a bit larger than the typical savegame's stockpile)
	// plus a small supply of cement and machine parts for efficiency
	fprintf(output, "\t\t\tstockpile=\n");
	fprintf(output, "\t\t\t{\n");
	fprintf(output, "\t\t\t\tcement=0.5");
	fprintf(output, "\t\t\t\tmachine_parts=0.05");
	for (map<string,float>::const_iterator itr = type->inputs.begin(); itr != type->inputs.end(); ++itr)
	{
		fprintf(output, "\t\t\t\t%s=%f", itr->first.c_str(), itr->second * 0.5);
	}
	fprintf(output, "\t\t\t}\n");

	// prime the pump with a little starting cash
	fprintf(output, "\t\t\tmoney=20000.0");

	fprintf(output, "\t\t}\n");
}


void V2FactoryFactory::loadRequiredTechs(string filename)
{
	Object* obj = doParseFile(filename.c_str());
	vector<Object*> techObjs = obj->getLeaves();
	for (vector<Object*>::iterator itr = techObjs.begin(); itr != techObjs.end(); ++itr)
	{
		std::vector<Object*> building = (*itr)->getValue("activate_building");
		for (vector<Object*>::iterator bitr = building.begin(); bitr != building.end(); ++bitr)
		{
			factoryTechReqs.insert(make_pair((*bitr)->getLeaf(), (*itr)->getKey()));
		}
	}
}


void V2FactoryFactory::loadRequiredInventions(string filename)
{
	Object* obj = doParseFile(filename.c_str());
	vector<Object*> invObjs = obj->getLeaves();
	for (vector<Object*>::iterator itr = invObjs.begin(); itr != invObjs.end(); ++itr)
	{
		std::vector<Object*> effect = (*itr)->getValue("effect");
		if (effect.size() == 0)
			continue;
		std::vector<Object*> building = effect[0]->getValue("activate_building");
		for (vector<Object*>::iterator bitr = building.begin(); bitr != building.end(); ++bitr)
		{
			factoryInventionReqs.insert(make_pair((*bitr)->getLeaf(), (*itr)->getKey()));
		}
	}
}


void V2FactoryFactory::init(string V2Loc)
{
	// load required techs/inventions
	loadRequiredTechs(V2Loc + "\\technologies\\army_tech.txt");
	loadRequiredTechs(V2Loc + "\\technologies\\commerce_tech.txt");
	loadRequiredTechs(V2Loc + "\\technologies\\culture_tech.txt");
	loadRequiredTechs(V2Loc + "\\technologies\\industry_tech.txt");
	loadRequiredTechs(V2Loc + "\\technologies\\navy_tech.txt");
	loadRequiredInventions(V2Loc + "\\inventions\\army_inventions.txt");
	loadRequiredInventions(V2Loc + "\\inventions\\commerce_inventions.txt");
	loadRequiredInventions(V2Loc + "\\inventions\\culture_inventions.txt");
	loadRequiredInventions(V2Loc + "\\inventions\\industry_inventions.txt");
	loadRequiredInventions(V2Loc + "\\inventions\\navy_inventions.txt");

	// load factory types
	Object* obj = doParseFile((V2Loc + "\\common\\production_types.txt").c_str());
	vector<Object*> factoryObjs = obj->getLeaves();
	for (vector<Object*>::iterator itr = factoryObjs.begin(); itr != factoryObjs.end(); ++itr)
	{
		V2FactoryType ft;
		ft.init(*itr);
		map<string,string>::iterator reqitr = factoryTechReqs.find(ft.name);
		if (reqitr != factoryTechReqs.end())
			ft.requireTech = reqitr->second;
		reqitr = factoryInventionReqs.find(ft.name);
		if (reqitr != factoryInventionReqs.end())
		{
			for (int i = 0; i <= naval_exercises; ++i)
			{
				if (reqitr->second == inventionNames[i])
				{
					ft.requireInvention = (inventionType)i;
					break;
				}
			}
		}
		factoryTypes[ft.name] = ft;
	}

	obj = doParseFile("starting_factories.txt");
	vector<Object*> top = obj->getValue("starting_factories");
	if (top.size() != 1)
	{
		log("Error: Could not load starting factory list!\n");
		printf("Error: Could not load starting factory list!\n");
		exit(-1);
	}
	vector<Object*> factories = top[0]->getLeaves();
	for (vector<Object*>::iterator itr = factories.begin(); itr != factories.end(); ++itr)
	{
		string factoryType = (*itr)->getKey();
		int count = atoi((*itr)->getLeaf().c_str());

		map<string, V2FactoryType>::iterator t = factoryTypes.find(factoryType);
		if (t == factoryTypes.end())
		{
			log("Error: Could not locate V2 factory type for starting factories of type %s!\n", factoryType.c_str());
			continue;
		}
		factoryCounts.push_back(pair<V2FactoryType*, int>(&(t->second), count));
	}
}


deque<V2Factory> V2FactoryFactory::buildFactories()
{
	deque<V2Factory> retval;
	for (vector<pair<V2FactoryType*, int>>::iterator itr = factoryCounts.begin(); itr != factoryCounts.end(); ++itr)
	{
		for (int i = 0; i < itr->second; ++i)
		{
			retval.push_back(V2Factory(itr->first));
		}
	}
	return retval;
}