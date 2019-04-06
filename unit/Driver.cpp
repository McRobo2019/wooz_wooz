#include "Driver.h"
#include "Logger.h"
#include "SoundPlayer.h"
#include "Timer.h"

Driver::Driver(Motor &left, Motor &right, LineTracer *ltracer, MotionSequencer *msequencer){

	navi = new Navigator(left, right);
	linetracer = ltracer;
	motionseq  = msequencer;
	change_state = 0;

	// シナリオファイルにはロボットの制御動作が順に定義されている
	// ライントレース系の動作の終了を定義する必要があり、地図情報に基づく距離による制御を行う。
	// オープンループ制御のモーションは指定の動作を行うだけなので、地図情報との対応は不要である。
	// scenario.defには、シナリオ番号、地図情報(番号)、動作モード、動作タイプを定義する。
	// シナリオ番号：ソフトウェアの動作としては意味はない。藤江さん設計との対応付けのため用意
	// 地図情報(番号): map_L/Rに定義された番号(-1の場合は開ループ動作)
	// 動作モード: ライントレーサかモーションシーケンサーのどちらの何の動作かを識別する番号
	// 動作タイプ: 動作モードに付随する識別番号

	FILE *fp;
	if ( (fp = fopen("/ev3rt/def/scenario_L.def", "r")) == NULL ) {
		print_new_linef("ERROR: can't read scenario L file");
	} else {
		char line[MAX_STR]={0};
		while(fgets(line, sizeof(line), fp)) {
			char *head = line;
			while ((*head) == ' ') { head++; }
			Scenario scene;
			if ( head[0] == '#'  ) {
				continue;
			} else if ( head[0] == '\n' ) {
				continue;
			} else {
				;
			}
			char *token = strtok( head, ",");
			scene.no= atoi(token);
			int count = 1;
			while ( token != NULL ) {
				token = strtok( NULL, "," );
				if ( token != NULL ) {
					switch (count++) {
						case 1: scene.map  = atoi(token); break;
						case 2: scene.mode = atoi(token); break;
						case 3: scene.type = atoi(token); break;
						default : break;
					}
				}
			}
			scenario.push_back(scene);
		}
		fclose(fp);
	}

	for(unsigned int  i = 0; i < scenario.size(); i++) {
		print_new_line("scenario[%d], map = %d, mode = %d, type = %d", 
			scenario[i].no, scenario[i].map, scenario[i].mode, scenario[i].type);
	}
	scncreator   = new ScenarioCreator(&scenario);
	cur_scenario =  0;  // 現在のシナリオ
	pre_scenario = -1;  // 実行中（過去）のシナリオ
	pre_power    =  0;  // 前回のモードのパワー
}

int Driver::drive() {

//	print_new_line("drive cur_scenario,%d",cur_scenario);

	// 開ループの場合は地図による区間判定を行わない
	if ( scenario[cur_scenario].map < 0 ) {
		// オープン走行中に割り込まれないよう周期タスクを止める
		//ev3_stp_cyc(EV3_CYC_TRACER);
		change_state = 1;

		sound_play_chime2(NOTE_C5, 50);
		sound_play_chime2(NOTE_C5, 50);

		// 現在のシナリオのモードを設定
		motionseq->set_move_mode(scenario[cur_scenario].mode, scenario[cur_scenario].type);

		print_new_line("scenario.no   = %d" ,scenario[cur_scenario].no);
		print_new_line("scenario.map  = %d" ,scenario[cur_scenario].map);
		print_new_line("scenario.mode = %d" ,scenario[cur_scenario].mode);
		print_new_line("scenario.type = %d" ,scenario[cur_scenario].type);

		// 前回走行時のパワーを与え、動作をスムースにつなぐ
		motionseq->setIniPower(pre_power);
		int cond = motionseq->drive();
		scncreator->update_scenario(cur_scenario, cond);

		// milageを初期化(milage = 0)
		navi->resetMilage(0);
		// 走行終了時のパワーを取得
		pre_power = motionseq->getLastPower();
		//		print_new_line("Last Power %f", pre_power);

		cur_scenario++;

	} else { // 地図情報がある場合は距離推定による判定を行う

		// 実行中のシナリオと現在のシナリオが違う場合モードを設定しなおす
		// 同じシナリオが2度設定されるのを防ぐためにモードの切り替えより先に設定する
		if ( pre_scenario != cur_scenario ) {
			// print_new_line("set scenario trace %d",cur_scenario);
			sound_play_chime2(NOTE_C4, 50);
			sound_play_chime2(NOTE_C4, 50);
			print_new_line("sound,%d", timer_now());

			// milageを初期化
			navi->resetMilage(0);
			print_new_line("reset_milage,%d", timer_now());

			// 現在のシナリオのモードを設定
			linetracer->set_move_mode(scenario[cur_scenario].mode, scenario[cur_scenario].type); //直線
			print_new_line("set_mode,%d", timer_now());
			pre_scenario = cur_scenario;

			print_new_line("scenario.no   = %d" ,scenario[cur_scenario].no);
			print_new_line("scenario.map  = %d" ,scenario[cur_scenario].map);
			print_new_line("scenario.mode = %d" ,scenario[cur_scenario].mode);
			print_new_line("scenario.type = %d" ,scenario[cur_scenario].type);

			// 前回走行時のパワーを与え、動作をスムースにつなぐ
			linetracer->setIniPower(pre_power);

			// モード切替が終了したので周期タスクを動かし始める
			change_state = 0;
			print_new_line("wake_tracer_task,%d", timer_now());
		}
		
		// 自分の位置とマップの位置が違う場合、次のモードに進め
		if ( navi->compute(scenario[cur_scenario].map) ) {

			// 次のモードへの切り替え前に割り込みが発生しないようにする
			change_state = 1;
			print_new_line("sleep_tracer_task,%d", timer_now());

			// 走行終了時のパワーを取得
			pre_power = linetracer->getLastPower();
			cur_scenario++;
		}
	}

	if ( cur_scenario < (int)scenario.size()) {
		return(0); //シナリオの処理継続中
	} else {
		print_new_line("scenario end"); 
		return(1); //シナリオを最後まで実行
	}
}

int Driver::goNextScenario() {
	pre_power = linetracer->getLastPower();
	change_state = 1;
	// モード遷移のためmilageを強制セット
	navi->setMilage(10000);
	sound_play_chime2(NOTE_A5, 50);
	sound_play_chime2(NOTE_A5, 50);
	return(0);
}

int Driver::getChangeState() {

	return change_state;

}
