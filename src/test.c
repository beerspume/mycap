#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

int canprint(uint8_t c){
    if(c>=32 && c<=126){
        return 1;
    }else{
        return 0;
    }
}

uint8_t converC(uint8_t c){
    if(canprint(c)){
        return c;
    }else{
        return '.';
    }
}

void flush_dis(int pos,char* dis_hex,char* dis_char,FILE* out_stream,int char_num_of_line){
    fprintf(out_stream,"%07X: %s",pos,dis_hex);
    int len=strlen(dis_hex);
    for(int i=0;i<(char_num_of_line*2+char_num_of_line/2)-len;i++){
        fprintf(out_stream," ");
    }
    fprintf(out_stream,"  %s\n",dis_char);
}

void show_hex(FILE* in_strean,FILE* out_stream,int char_num_of_line){
    int BUFF_SIZE=1024;
    uint8_t* buff=malloc(BUFF_SIZE);

    char dis_hex[char_num_of_line*2+char_num_of_line/2];
    char dis_char[char_num_of_line+1];
    char* p_dis_hex=dis_hex;
    char* p_dis_char=dis_char;

    int read_len;

    int pos_of_line=0;
    unsigned long int pos=0;
    while((read_len=fread(buff,1,BUFF_SIZE,in_strean))>0){
        for(int i=0;i<read_len;i++){
            if(pos_of_line!=0 && pos_of_line%2==0){
                sprintf(p_dis_hex++," ");
            }
            sprintf(p_dis_hex,"%02X",buff[i]);
            p_dis_hex+=2;

            sprintf(p_dis_char++,"%c",converC(buff[i]));

            pos_of_line++;

            if(pos_of_line>=char_num_of_line){
                flush_dis(pos,dis_hex,dis_char,out_stream,char_num_of_line);
                pos+=char_num_of_line;
                pos_of_line=0;
                p_dis_hex=dis_hex;
                p_dis_char=dis_char;
            }
        }
    }
    flush_dis(pos,dis_hex,dis_char,out_stream,char_num_of_line);
    fprintf(out_stream,"\n");
    free(buff);

}

int main(int argc, char *argv[]){
    int ch;
    int arg_len=16;
    while((ch=getopt(argc,argv,"l:"))!=-1){
        switch(ch){
            case 'l':
                arg_len=atoi(optarg);
                break;
            default:
                break;
        }
    };

    show_hex(stdin,stdout,arg_len);

    return 0;
}