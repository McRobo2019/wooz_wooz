#include "Map.h"
#include "Logger.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

Map::Map(){
	
	FILE *fp;
	
	if ( (fp = fopen("/ev3rt/def/map_L.def", "r")) == NULL ) {
		print_new_linef("ERROR: can't read L Map file");
	} else {
		char line[MAX_STR]={0};
		char  *id = NULL;
		char  *d  = NULL;
		int   sqid = 0;
		float val  = 0;
		
		while(fgets(line, sizeof(line), fp)) {

			char *head=line;
			while ((*head) == ' ') { head++; }
			// 先頭の文字が＃もしくは改行だったら読み飛ばす
			if ( head[0] == '#' ) {
				continue;
			} else if ( head[0] == '\n' ) {
				continue;
			} else {
				;
			}

			id  = strtok(head, ",");
			if ( (sqid = atoi(id)) != -1 ) {
				d   = strtok(NULL, ",");
				val = atof(d);
				point[sqid] = val;
			}
		}
		fclose(fp);
	}

	unordered_map<int,float>::iterator it;
	for(it = point.begin(); it != point.end(); it++) {
		print_new_line("map[%d] point = %f", it->first, it->second);
	}
	
	surplus = 0.0;
}

int Map::is_in_section(int map_no, float milage) {
	// 区間毎の相対距離が指定距離を超えたかを判定
	print_new_line("map_no = %d, milage = %f, point[map_no] = %f", map_no, milage, point[map_no]);
	if ( milage > point[map_no] ) {
		surplus = milage - point[map_no]; // 実際の走行距離と目標距離の差
		return (1);
	}
	return 0;
}

float Map::getSurplus() {
	return(surplus);		// 実際の走行距離と目標距離の差
}
