#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_


#include "EU3Province.h"
#include "EU3Relations.h"
#include "EU3Army.h"


class EU3Country
{
	public:
		void						init(Object* obj);
		string					getTag();
		void						addProvince(EU3Province*);
		void						clearProvinces();
		void						addCore(EU3Province*);
		void						clearCores();
		vector<EU3Province*>	getProvinces();
		vector<EU3Province*>	getCores();
		int						getCapital();
		string					getTechGroup();
		string					getPrimaryCulture();
		vector<string>			getAcceptedCultures();
		string					getReligion();
		void						setPrestige(double);
		double					getPrestige();
		void						setLandTech(double);
		double					getLandTech();
		void						setNavalTech(double);
		double					getNavalTech();
		void						setTradeTech(double);
		double					getTradeTech();
		void						setProductionTech(double);
		double					getProductionTech();
		void						setGovernmentTech(double);
		double					getGovernmentTech();
		vector<string>			getFlags();
		vector<string>			getModifiers();
		bool						getPossibleDaimyo();
		string					getGovernment();
		vector<EU3Relations>	getRelations();
		vector<EU3Army>		getArmies();
		void						resolveRegimentTypes(const RegimentTypeMap& map);
		int						getCentralizationDecentralization();
		int						getAristocracyPlutocracy();
		int						getSerfdomFreesubjects();
		int						getInnovativeNarrowminded();
		int						getMercantilismFreetrade();
		int						getOffensiveDefensive();
		int						getLandNaval();
		int						getQualityQuantity();
	private:
		string					tag;
		vector<EU3Province*>	provinces;
		vector<EU3Province*>	cores;
		int						capital;
		string					techGroup;
		string					primaryCulture;
		vector<string>			acceptedCultures;
		string					religion;
		double					prestige;
		double					landTech;
		double					navalTech;
		double					tradeTech;
		double					productionTech;
		double					governmentTech;
		vector<string>			flags;
		vector<string>			modifiers;
		bool						possibleDaimyo;
		string					government;
		vector<EU3Relations>	relations;
		vector<EU3Army>		armies; // and navies...
		int						centralization_decentralization;
		int						aristocracy_plutocracy;
		int						serfdom_freesubjects;
		int						innovative_narrowminded;
		int						mercantilism_freetrade;
		int						offensive_defensive;
		int						land_naval;
		int						quality_quantity;
};


#endif // EU3COUNTRY_H_