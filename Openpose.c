#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "json.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

//dirty hacks for higher than implementation
double coor_x[100][8]={[0 ... 99][0 ... 7] = 16384}, coor_y[100][8]={[0 ... 99][0 ... 7] = 16384}, coor_int[100][8]={[0 ... 99][0 ... 7] = 16384};
int lefthand[100]={[0 ... 99] = 0},righthand[100]={[0 ... 99] = 0}, hand_int[100]={[0 ... 99] = 0},leftleg[100]={[0 ... 99] = 0},rightleg[100]={[0 ... 99] = 0};
int posture0[100]={[0 ... 99] = 0},posture1_x[100]={[0 ... 99] = 0},posture1_y[100]={[0 ... 99] = 0};
int posture2[100]={[0 ... 99] = 0};
int posture3_x[100]={[0 ... 99] = 0},posture3_y[100]={[0 ... 99] = 0};
//implement if needed
//double coor_x_old[100][4]={0}, coor_y_old[100][4]={0};


int num = -1;
int num_old = 0;
char defaultfilename[16] = "_keypoints.json";
char prependfilename[128] = "/home/nancy493501/openpose_raise_hand-master/json/";
char filenamestring[13] = "0000000000000";
char filename[128] = "/home/nancy493501/openpose_raise_hand-master/json/000000000000_keypoints.json";
long long int file_i =0;
time_t old_result = 0;
time_t result;
/* 
gcc main.c json.c -lm
 */
static void print_depth_shift(int depth)
{
        int j;
        for (j=0; j < depth; j++) {
		//printf(" ");
        }
}
static void output();
char* body_parts(int x);
static void coorx(json_value* value, int x, int y);
static void coory(json_value* value, int x, int y);
static void spit(json_value* value, int x, int y);
static void process_value(json_value* value, int depth, int x);

static void process_object(json_value* value, int depth)
{
        int length, x;
        if (value == NULL) {
                return;
        }
        length = value->u.object.length;
        for (x = 0; x < length; x++) {
                print_depth_shift(depth);
                //printf("object[%d].name = %s\n", x, value->u.object.values[x].name);
                process_value(value->u.object.values[x].value, depth+1, x);
        }
}

static void process_array(json_value* value, int depth)
{
        int length, x;
        if (value == NULL) {
                return;
        }
        length = value->u.array.length;
        //printf("array: %d\n", depth);
        for (x = 0; x < length; x++) {
                process_value(value->u.array.values[x], depth, x);
        }
}

static void process_value(json_value* value, int depth, int x)
{
        int j;
	char a[100];
        if (value == NULL) {
                return;
        }
        if (value->type != json_object) {
                print_depth_shift(depth);
        }
        switch (value->type) {
                case json_none:
//                printf("none\n");
                        break;
                case json_object:
                        process_object(value, depth+1);
                        break;
                case json_array:
                        process_array(value, depth+1);
                        break;
                case json_integer:
//                        printf("int: %10" PRId64 "\n", value->u.integer);
                        break;
                case json_double:
                        //printf("double: %f\n", value->u.dbl);
					if(x==0)num++;
					//printf("%d\n",x);

                                        //判斷身體的點位右手：2 3 左手：5 6 右腳：8 9 左腳:11 12
					if(x/3==2 || x/3==3 || x/3==5 || x/3==6 || x/3==9 || x/3==10 || x/3==12 || x/3==13){ 						
                                                 switch(x/3){
							case 2:spit(value, x, 0);break;
							case 3:spit(value, x, 1);break;
							case 5:spit(value, x, 2);break;
							case 6:spit(value, x, 3);break;
							case 9:spit(value, x, 4);break;
							case 10:spit(value, x, 5);break;
							case 12:spit(value, x, 6);break;
							case 13:spit(value, x, 7);break;
							default:break;
						}
					}
					//if(num > num_old){
						//printf("hooman[%d], y2=%f, y3=%f, y5=%f, y6=%f\n",num,coor_y[num][0],coor_y[num][1],coor_y[num][2],coor_y[num][3]);
						//num_old++;
					//}
					break;
                case json_string:
//                        printf("string: %s\n", value->u.string.ptr);
                        break;
                case json_boolean:
//                        printf("bool: %d\n", value->u.boolean);
                        break;
        }
}
char* body_parts(int x){
	switch(x/3){
		case 2:return "RShoulder";
		case 3:return "RElbow";
		case 5:return "LShoulder";
		case 6:return "LElbow";
		default:return "Unimplemented";
	}
}

static void spit(json_value* value, int x, int y){
	switch(x%3){
		case 0:coorx(value, x, y);break;
		case 1:coory(value, x, y);break;
		case 2:
		       //confidence, used as counter
		       //printf("x: %d, This is c: %f!\n",x/3,value->u.dbl);
		       //if(y==3)num++;
			   //printf("Y: %d\n",y);
			   break;
		default:break;
	}
}



static void coorx(json_value* value, int x, int y){
	char* part_name=body_parts(x);
	//doing YOLO
	//if(coor_x[num][y]!=0)coor_x_old[num][y]=coor_x[num][y];
	coor_x[num][y]=value->u.dbl;
	printf("x: %d, coor_x[%d][%d] = %f\n",x/3,num,y,coor_x[num][y]);
	printf("Person[%d] %s xcoor=%f, x=%d\n",num,part_name,coor_x[num][y],x);
	
	
}
static void coory(json_value* value, int x, int y){
char* part_name=body_parts(x);
	//char* part_name=body_parts(x);
	//YOLO
    //if(coor_y[num][y]!=0)coor_y_old[num][y]=coor_y[num][y];
    coor_y[num][y]=value->u.dbl;
        
        
	
        //printf("x2=%f, x3=%f, x5=%f, x6=%f\n",coor_x[num][0],coor_x[num][1],coor_x[num][2],coor_x[num][3]);
	// 判斷手舉起來
     /*   if(coor_y[num][0]-coor_y[num][1]>0.5)righthand[num]=1;
	if(coor_y[num][2]-coor_y[num][3]>0.5)lefthand[num]=1;
	
        // 判斷腳舉起來
        if(coor_y[num][4]-coor_y[num][5]>0.5)rightleg[num]=1;
	if(coor_y[num][6]-coor_y[num][7]>0.5)leftleg[num]=1;
	*/
	
	//if(coor_y[num][0]>0 && coor_y[num][1]>0)posture0[num]=1;

	
        printf("y: %d, coor_y[%d][%d] = %f\n",x/3,num,y,coor_y[num][y]);
	printf("Person[%d] %s ycoor=%f, x=%d\n",num,part_name,coor_y[num][y],x);
}

static void output(){
	if(num==0)return;

        printf("==DEBUG== Human[%d], x2=%f, x3=%f, x5=%f, x6=%f ==DEBUG==\n",num,coor_x[num][0],coor_x[num][1],coor_x[num][2],coor_x[num][3]);
	printf("==DEBUG== Human[%d], x8=%f, x9=%f, x11=%f, x12=%f ==DEBUG==\n",num,coor_x[num][4],coor_x[num][5],coor_x[num][6],coor_x[num][7]);


        printf("==DEBUG== Human[%d], y2=%f, y3=%f, y5=%f, y6=%f ==DEBUG==\n",num,coor_y[num][0],coor_y[num][1],coor_y[num][2],coor_y[num][3]);
	printf("==DEBUG== Human[%d], y8=%f, y9=%f, y11=%f, y12=%f ==DEBUG==\n",num,coor_y[num][4],coor_y[num][5],coor_y[num][6],coor_y[num][7]);

	//DO SOMETHING HERE!!!!!!
	result = time(NULL);
	

        // x 軸的判斷
        // 姿勢一
	if((coor_x[num][7]-coor_x[num][6] < 10 && coor_x[num][7]-coor_x[num][6] > -10) || (coor_x[num][5]-coor_x[num][4] < 10 && coor_x[num][5]-coor_x[num][4] > -10))posture1_x[num] = 1;

        // 姿勢二 
	if(coor_x[num][1]-coor_x[num][0] < 10 && coor_x[num][1]-coor_x[num][0] > -10 && coor_x[num][3]-coor_x[num][2] < 10 && coor_x[num][3]-coor_x[num][2] > -10)posture2[num]=1;
	//姿勢三
	if(coor_x[num][7]-coor_x[num][6]>30 || coor_x[num][4]-coor_x[num][5]>30){
		posture3_x[num] = 1;
                printf("get posture3_x\n");
	}


        // y 軸的判斷
        // 姿勢一
	if(posture1_x[num] == 1 && coor_y[num][0]-coor_y[num][1]>20 && coor_y[num][2]-coor_y[num][3]>20 && coor_y[num][5]-coor_y[num][4]<150 && coor_y[num][5]-coor_y[num][4]>-50 )posture1_y[num]=1;	
	//姿勢三
	if(posture3_x[num] == 1 && coor_y[num][0]-coor_y[num][1]<10 && coor_y[num][0]-coor_y[num][1]>-10 && coor_y[num][2]-coor_y[num][3]<10 && coor_y[num][2]-coor_y[num][3]>-10 && coor_y[num][4]-coor_y[num][5]<20 && coor_y[num][4]-coor_y[num][5]>-20/* || (coor_y[num][4]-coor_y[num][5]<20 && coor_y[num][4]-coor_y[num][5]>-20))*/)posture3_y[num]=1;


	if(posture1_x[num] == 1 && posture1_y[num] == 1)printf("人類 %d 舉起了雙手跟右腳  姿勢一正確@ %s！\n", num, ctime(&result));
	if(posture2[num])printf("人類 %d 姿勢二正確@ %s！\n", num, ctime(&result));
	if(posture3_x[num] == 1 && posture3_y[num] == 1)printf("人類 %d 手平舉並且右大腿水平  姿勢三正確@ %s！\n", num, ctime(&result));
	
}




int main(int argc, char** argv){
	FILE *fp;
	struct stat filestatus;    // stat 為內建函數 需 #include<sys/stat.h> 用來獲取指定文件或是文件夾的訊息 正確會returrn 0 錯誤會return -1  
	int file_size;
	long long int file_i = 0;
	char* file_contents;
	int pollingDelay = 100;
	json_char* json;
	json_value* value;         //json_value 這個structure 在 json.h 裡面
	
	for(file_i = 0;file_i < 999999999999; file_i++){
		// find file
		sprintf(filenamestring, "%012lld", file_i);      // put file_i in filenamestring
		strcpy(filename, prependfilename);               // copy the path of json (/home/e516/openpose_raise_hand/json/) to filename 
		strcat(filename, filenamestring);                // add filenamestring behind filename => filename = filename + filenamestring (str 的用法)
		strcat(filename, defaultfilename);               // compose of complete filename => /home/e516/openpose_raise_hand/json/ + filenamestring + defaulefilename
		//if file not found
		if ( stat(filename, &filestatus) != 0) {
			//printf("Filename: %s\n", filename);
			file_i--;
			#ifdef _WIN32
			Sleep(pollingDelay);
			#else
			usleep(pollingDelay*1000);                   // usleep 和 sleep 差別在於時間單位 sleep 是 s usleep 是 us 10^-6
			#endif
		}
		else{
                        printf("Filename: %s\n", filename);
			//if OOM
			file_size = filestatus.st_size;
			file_contents = (char*)malloc(filestatus.st_size);
			if ( file_contents == NULL) {                                                 
				fprintf(stderr, "Memory error: unable to allocate %d bytes\n", file_size);     //記憶體沒有分配空間給file_sonntents
				return 1;
			}
			
			//if no read permission
			fp = fopen(filename, "rt");
			if (fp == NULL) {
				fprintf(stderr, "Unable to open %s\n", filename);
				fclose(fp);
				free(file_contents);
				return 1;
			}
			//if unable to read for some reason
			if ( fread(file_contents, file_size, 1, fp) != 1 ) {
				fprintf(stderr, "Unable to read content of %s\n", filename);
			}
			fclose(fp);
			
			//printf("%s\n", file_contents);
			
			json = (json_char*)file_contents;
			
			value = json_parse(json,file_size);                  //json_parse 為內建函數, 會把一個JSON字串轉換成 JavaScript的數值或是物件
			
			if (value == NULL) {
				fprintf(stderr, "Unable to parse data\n");
			}
			
			process_value(value, 0, 0);                          // 主要的function
                        output();			


			json_value_free(value);                              //json.c 裡面的 function 
			free(file_contents);
			
			/*#ifdef _WIN32
			Sleep(pollingDelay);
			#else
			usleep(pollingDelay*1000);
			#endif
			*/
		    // 刪掉filename 並把value初始化
			//remove(filename);
			for(int l=0; l<=num; l++){
				for(int n=0; n<8; n++){
					coor_y[l][n] = 16384.0;
					coor_x[l][n] = 16384.0;
				}
				lefthand[l] = 0;
				righthand[l] = 0;
				leftleg[l] = 0;
				rightleg[l] = 0;
				posture0[l] = 0;
				posture1_x[l] = 0;
                                posture1_y[l] = 0;
				posture2[l] = 0;
				posture3_x[l] = 0;
                                posture3_y[l] = 0;
			}
			num = -1;
			num_old = 0;
				}
			}

	return 0;
}
