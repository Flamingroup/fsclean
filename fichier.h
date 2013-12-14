#ifndef FICHIER_H
#define FICHIER_H

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

using namespace std;

class Fichier
{
	private:
		int id;
		path cheminfic;
		string MD5;
		string filenameTrime;

	public:
		Fichier(path p);
		void updateIntoDB();
};

#endif // FICHIER_H
