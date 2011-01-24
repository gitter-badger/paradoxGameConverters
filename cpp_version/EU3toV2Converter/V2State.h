#ifndef V2Province_H_
#define V2Province_H_

#include "V2Province.h"
#include <vector>
using namespace std;


class V2State
{
	public:
		V2State(int newId);
		void addProvince(V2Province);
	private:
		int id;
		vector<V2Province> provinces;
};


#endif	// V2Province_H_