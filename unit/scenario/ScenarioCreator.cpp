#include "ScenarioCreator.h"
#include "Logger.h"

#define SCN_MAPL_READ_COLOR (35)

ScenarioCreator::ScenarioCreator(vector<Scenario> *scn):scenario(scn) {
	;
}

int ScenarioCreator::update_scenario(int scn_no, int condition) {

	//予め決められたシナリオの動作結果に基づいてシナリオの編集動作を行う．
	//現状はシナリオ番号，条件に基づいたハードコードを行う為，L/Rでシナリオ
	//番号を変える必要がある．
	print_new_line("update start");
	logger_flush();

	// Lコース：カラーパターンを読んで，同色ブロックを押すシナリオを削除する
	if ( (*scenario)[scn_no].no == SCN_MAPL_READ_COLOR ) {

		print_new_line("scenario no = %d", (*scenario)[scn_no].no );
		logger_flush();

		vector <int> del_scn;
		int unknown = 0;
		switch(condition) {
			// 黄色
			case 4: del_scn.push_back( 77);
					del_scn.push_back( 78);
					del_scn.push_back( 83);
					del_scn.push_back( 91);break;

			// 緑色
			case 3: del_scn.push_back( 96);
					del_scn.push_back( 97);
					del_scn.push_back(100);
					del_scn.push_back(101);
					del_scn.push_back(115);
					del_scn.push_back(116);break;

			// 青色
			case 2: del_scn.push_back(127);
					del_scn.push_back(128);
					del_scn.push_back(133);
					del_scn.push_back(141);break;

			// 赤色
			case 5: del_scn.push_back(144);
					del_scn.push_back(145);
					del_scn.push_back(150);
					del_scn.push_back(151);
					del_scn.push_back(165);
					del_scn.push_back(166);break;
			default:
					unknown = 1;
					print_new_line("undefined color = %d", condition);
					break;
		}

		if (unknown == 0) {

			int count = 0;
			for ( unsigned int i = 0; i < scenario->size(); i++ ) {
				if ( (*scenario)[i].no == del_scn[count] ) {
					scenario->erase(scenario->begin()+i);
					if ( count < (int)(del_scn.size() - 1)) { count++; }
					i--;
					print_new_line("erace scenario index = %d, no = %d", i ,(*scenario)[i].no) ;
				}
			}

		}

	}

	return 0;
}

int ScenarioCreator::insert_block_scenario() {

	motiontrans.set_scenario(scenario);
	motiontrans.insert_scenario(-1); //シナリオ番号は要調整

	return 0;
}
