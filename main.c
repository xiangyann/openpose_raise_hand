#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "json.h"

double coor_x[100][4]={0},coor_x_old[100][4]={0};
double coor_y[100][4]={0},coor_y_old[100][4]={0};
int num = 0;

/* gcc a.c json.c -lm */
static void print_depth_shift(int depth)
{
        int j;
        for (j=0; j < depth; j++) {
		//printf(" ");
        }
}
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
			if(x/3==2 || x/3==3 || x/3==5 || x/3==6){
				switch(x/3){
					case 2:spit(value, x, 0);break;
					case 3:spit(value, x, 1);break;
					case 5:spit(value, x, 2);break;
					case 6:spit(value, x, 3);break;
					default:break;
				}
			}
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
		default:return "Invalid";
	}
}
static void coorx(json_value* value, int x, int y){
	char* part_name=body_parts(x);
	if(coor_x[num][y]!=0)coor_x_old[num][y]=coor_x[num][y];
	coor_x[num][y]=value->u.dbl;
	//printf("x: %d, coor_x[%d][%d] = %f\n",x/3,num,y,coor_x[num][y]);
	printf("Person[%d] %s x=%f",num,part_name,coor_x[num][y]);
}
static void coory(json_value* value, int x, int y){
	//char* part_name=body_parts(x);
        if(coor_y[num][y]!=0)coor_y_old[num][y]=coor_y[num][y];
        coor_y[num][y]=value->u.dbl;
        //printf("x: %d, coor_x[%d][%d] = %f\n",x/3,num,y,coor_y[num][y]);
	printf(" ,y=%f\n",coor_y[num][y]);
}

static void spit(json_value* value, int x, int y){
	switch(x%3){
		case 0:coorx(value, x, y);break;
		case 1:coory(value, x, y);break;
		case 2:
		       //confidence, used as counter
		       //printf("x: %d, This is c: %f!\n",x/3,value->u.dbl);
		       if(y==3)num++;break;
		default:break;
	}
}


int main(int argc, char** argv)
{
        char* filename;
        FILE *fp;
        struct stat filestatus;
        int file_size;
        char* file_contents;
        json_char* json;
        json_value* value;

        if (argc != 2) {
                fprintf(stderr, "%s <file_json>\n", argv[0]);
                return 1;
        }
        filename = argv[1];

        if ( stat(filename, &filestatus) != 0) {
                fprintf(stderr, "File %s not found\n", filename);
                return 1;
        }
        file_size = filestatus.st_size;
        file_contents = (char*)malloc(filestatus.st_size);
        if ( file_contents == NULL) {
                fprintf(stderr, "Memory error: unable to allocate %d bytes\n", file_size);
                return 1;
        }

        fp = fopen(filename, "rt");
        if (fp == NULL) {
                fprintf(stderr, "Unable to open %s\n", filename);
                fclose(fp);
                free(file_contents);
                return 1;
        }
        if ( fread(file_contents, file_size, 1, fp) != 1 ) {
                fprintf(stderr, "Unable t read content of %s\n", filename);
                fclose(fp);
                free(file_contents);
                return 1;
        }
        fclose(fp);

        printf("%s\n", file_contents);

        printf("--------------------------------\n\n");

        json = (json_char*)file_contents;

        value = json_parse(json,file_size);

        if (value == NULL) {
                fprintf(stderr, "Unable to parse data\n");
                free(file_contents);
                exit(1);
        }

        process_value(value, 0, 0);

        json_value_free(value);
        free(file_contents);
        return 0;
}