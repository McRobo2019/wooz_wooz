#ifndef COMMAND_H_
#define COMMAND_H_

#include "common.h"
#include "Motion.h"
#include <vector>

using namespace std;

class Command : public Motion {

	public:
		Command(Volante *vlnt, StopWatch *watch):Motion(vlnt, watch){;}

		void setMotion(Motion *m) {
			motion_list.push_back(m);
		}

		virtual int drive() {
			int status = 0;
			for (unsigned  int i = 0; i < motion_list.size(); i++ ) {
				if ((status = motion_list[i]->drive()) != 0) {
					return status;
				}
			}
			return status;
		}

	private:
		vector<Motion*> motion_list;
};

#endif // COMMAND_H_
