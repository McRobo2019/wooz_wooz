#include "MotionTranslator.h"

#define EV3RT_FILE_PATH "/ev3rt/def/block_convey.def"
#define LOCAL_FILE_PATH "block_convey.def"

MotionTranslator::MotionTranslator(){
	dir = new Pos(0,0);
	set_motion();
	read_path_file();
}

void MotionTranslator::set_scenario(vector<Scenario> *scn) {
	course_scenario = scn;
}

int MotionTranslator::read_path_file() {

	FILE *fp;
	// ファイルパスは開発環境、ロボットで変更が必要
	if ( (fp = fopen(LOCAL_FILE_PATH, "r")) == NULL ) {
		print_new_line("ERROR: can't read block convey file");
	} else {
		char line[MAX_STR]={0};
		char *token;
		int no, x, y;
		vector <Pos> *path;
		Pos pos(0,0);

		// 姿勢の読み込み
		while (fgets(line, sizeof(line), fp)) {
			if (line[0] != '#') break;
		}
		token = strtok(line, ",");
		x = atoi(token);
		token = strtok( NULL, "," );
		y = atoi(token);
		dir->set(x,y);

		print_new_line("dir x = %d, y = %d", dir->x, dir->y);

		// 移動パスの読み込み
		while(fgets(line, sizeof(line), fp)) {

			if ( line[0] == '#'  ) {
				continue;
			} else if ( line[0] == '\n' ) {
				continue;
			} else {
				;
			}

			char *token = strtok(line, ",");
			no= atoi(token);
			switch (no) {
				case 0: path = &convpath.enter;         break;
				case 1: path = &convpath.hold_block[0]; break;
				case 2: path = &convpath.conv_block[0]; break;
				case 3: path = &convpath.hold_block[1]; break;
				case 4: path = &convpath.conv_block[1]; break;
				case 5: path = &convpath.hold_block[2]; break;
				case 6: path = &convpath.conv_block[2]; break;
				case 7: path = &convpath.hold_block[3]; break;
				case 8: path = &convpath.conv_block[3]; break;
				case 9: path = &convpath.exit; break;
				default: break;
			}

			int count = 1;
			while ( token != NULL ) {
				token = strtok( NULL, "," );
				if ( token != NULL ) {
					switch (count++) {
						case 1: pos.x = atoi(token); break;
						case 2: pos.y = atoi(token); break;
						default : break;
					}
				}
			}
			path->push_back(pos);
			print_new_line("no = %d, x = %d, y = %d", no, pos.x, pos.y );
		}
		fclose(fp);
	}

	convpath.debug_print();
	return 0;
}

int MotionTranslator::insert_scenario(int pre_sno) {

	translate(0, convpath.enter);
	translate(1, convpath.hold_block[0]);
	translate(2, convpath.conv_block[0]);
	translate(1, convpath.hold_block[1]);
	translate(2, convpath.conv_block[1]);
	translate(1, convpath.hold_block[2]);
	translate(2, convpath.conv_block[2]);
	translate(1, convpath.hold_block[3]);
	translate(2, convpath.conv_block[3]);
	translate(0, convpath.exit);

	for (unsigned int i = 0; i < course_scenario->size(); i++) {
		print_new_line("course_scenario no = %d", (*course_scenario)[i].no);
		if ( (*course_scenario)[i].no == pre_sno ) {
			course_scenario->insert(course_scenario->begin() + i + 1, 
					                block_scenario.begin(),
									block_scenario.end());
			break;
		}
	}

	return 0;
}

int MotionTranslator::translate(int type, vector<Pos> &path) {

	for (unsigned int i = 0; i < path.size(); i++) {
		print_new_line("%d: x = %d, y = %d", i, path[i].x, path[i].y);
	}

	Pos vec(0,0);
	int dot_product   = 0;
	int cross_product = 0;
	for (unsigned int i = 0; i < path.size()-1; i++) {
		vec.set(path[i+1].x - path[i].x, path[i+1].y - path[i].y);
		dot_product   = dir->x * vec.x + dir->y * vec.y; //内積
		cross_product = dir->x * vec.y - dir->y * vec.x; //外積
		print_new_line("%d: path: dir(%d, %d)->next(%d, %d)", i, path[i].x, path[i].y, path[i+1].x, path[i+1].y);
		print_new_line("%d: vec : dir(%d, %d)->next(%d, %d)", i, dir->x, dir->y, vec.x, vec.y);
		print_new_line("%d: dot product = %d, cross product = %d", i, dot_product, cross_product);

		if (dot_product == 0) {
			if (cross_product == 1) {  //右ターン
				print_new_line(" no = %d, map = %d, mode = %d, type = %d",
						turn_right[0].no, turn_right[0].map, turn_right[0].mode, turn_right[0].type);
				block_scenario.insert(block_scenario.end(), turn_right.begin(), turn_right.end());
			}
			if (cross_product == -1) { //左ターン
				block_scenario.insert(block_scenario.end(), turn_left.begin(), turn_left.end());
			}
			dir->set(vec.x, vec.y);
		} else if (dot_product == 1) { //前進
			block_scenario.insert(block_scenario.end(), forward.begin(), forward.end());
		} else { //後退禁止
			print_new_line("can't back travel...");
		}

	}

	if ( type == 1 ) {

		block_scenario.insert(block_scenario.end(), hold.begin(), hold.end());

	} else if ( type == 2 ) {

		block_scenario.insert(block_scenario.end(), release.begin(), release.end());
		dir->x = dir->x * (-1); dir->y = dir->y * (-1);

	}

	return 0;
}

void MotionTranslator::debug_print() {

	for (unsigned int i = 0; i < block_scenario.size(); i++ ) {
		switch ( block_scenario[i].no) {
			case 1030: print_new_line("[%d] forward:", i); break;
			case 1040: print_new_line("[%d] R turn :", i); break;
			case 1050: print_new_line("[%d] L turn :", i); break;
			case 1070: print_new_line("[%d] hold   :", i); break;
			case 1060: print_new_line("[%d] release:", i); break;
			default: break;
		}
		print_new_line(" no = %d, map = %d, mode = %d, type = %d",
				block_scenario[i].no, block_scenario[i].map, block_scenario[i].mode, block_scenario[i].type);
	}
}

void MotionTranslator::terminal_print() {

	int sno = 1100;
	for (unsigned int i = 0; i < block_scenario.size(); i++ ) {
		switch ( block_scenario[i].no) {
			case 1030: print("# forward:\n"); break;
			case 1040: print("# R turn :\n"); break;
			case 1050: print("# L turn :\n"); break;
			case 1070: print("# hold   :\n"); break;
			case 1060: print("# release:\n"); break;
			default: break;
		}
		print(" %d, %2d, %2d, %2d\n",
				sno++, block_scenario[i].map, block_scenario[i].mode, block_scenario[i].type);
	}
}

void MotionTranslator::set_motion () {

	Scenario scene;
	vector<Scenario> *motion;

	// 前進
	motion = &forward;
	scene.set(1030, -1, 10, 6);
	motion->push_back(scene);
	scene.set(1031, 30,  0, 1);
	motion->push_back(scene);
	scene.set(1032, 31,  3, 2);
	motion->push_back(scene);
	for (unsigned int i = 0; i < motion->size(); i++ ) {
		print_new_line("forward %d: no = %d, map = %d, mode = %d, type = %d",
				i, (*motion)[i].no, (*motion)[i].map, (*motion)[i].mode, (*motion)[i].type);
	}

	// 右ターン
	motion = &turn_right;
	scene.set(1040, -1, 11, 2);
	motion->push_back(scene);
	scene.set(1041, 41,  0, 1);
	motion->push_back(scene);
	scene.set(1042, 42,  3, 2);
	motion->push_back(scene);
	for (unsigned int i = 0; i < motion->size(); i++ ) {
		print_new_line("turn_r  %d: no = %d, map = %d, mode = %d, type = %d",
				i, (*motion)[i].no, (*motion)[i].map, (*motion)[i].mode, (*motion)[i].type);
	}

	// 左ターン
	motion = &turn_left;
	scene.set(1050, -1, 11, 3);
	motion->push_back(scene);
	scene.set(1051, 51,  0, 1);
	motion->push_back(scene);
	scene.set(1052, 52,  3, 2);
	motion->push_back(scene);
	for (unsigned int i = 0; i < motion->size(); i++ ) {
		print_new_line("turn_l  %d: no = %d, map = %d, mode = %d, type = %d",
				i, (*motion)[i].no, (*motion)[i].map, (*motion)[i].mode, (*motion)[i].type);
	}

	// ブロック掴む
	motion = &hold;
	scene.set(1070, -1, 22, 0);
	motion->push_back(scene);
	for (unsigned int i = 0; i < motion->size(); i++ ) {
		print_new_line("hold    %d: no = %d, map = %d, mode = %d, type = %d",
				i, (*motion)[i].no, (*motion)[i].map, (*motion)[i].mode, (*motion)[i].type);
	}

	// ブロック置く
	motion = &release;
	scene.set(1060, 60,  0, 2);
	motion->push_back(scene);
	scene.set(1061, -1, 12, 1);
	motion->push_back(scene);
	scene.set(1062, 62, 10, 8);
	motion->push_back(scene);
	scene.set(1063, 63,  0, 3);
	motion->push_back(scene);
	scene.set(1064, 64,  0, 1);
	motion->push_back(scene);
	scene.set(1065, 65,  3, 2);
	motion->push_back(scene);
	for (unsigned int i = 0; i < motion->size(); i++ ) {
		print_new_line("release %d: no = %d, map = %d, mode = %d, type = %d",
				i, (*motion)[i].no, (*motion)[i].map, (*motion)[i].mode, (*motion)[i].type);
	}
}
