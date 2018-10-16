#include "AES_tables.h"
#include <string.h>

unsigned char w[44][4];           //10������Կ

unsigned char SBox_Chg(char sin,int bits) {
//S�б任
	int i,j,k=0;
	char temp,out[8];
	for(j=7; j>=0; j--) { //����к��к�
		temp=sin&(1<<j);
		if(0 == temp) {
			out[k]=0;
			k++;
		} else {
			out[k]=1;
			k++;
		}
	}
	i=out[0]*8+out[1]*4+out[2]*2+out[3];  //�к�
	j=out[4]*8+out[5]*4+out[6]*2+out[7];  //�к�
	return (S_box[i][j]);   //���صõ���S�е�ֵ
}

unsigned char SBox_1_Chg(char sin,int bits) {
//S����任
	int i,j,k=0;
	char temp,out[8];
	for(j=7; j>=0; j--) {
		temp=sin&(1<<j);
		if(0 == temp) {
			out[k]=0;
			k++;
		} else {
			out[k]=1;
			k++;
		}
	}
	i=out[0]*8+out[1]*4+out[2]*2+out[3];//�к�
	j=out[4]*8+out[5]*4+out[6]*2+out[7];//�к�
	return (SBox_1[i][j]);

}

void Getkey(unsigned char key[16]) {
	//10������Կ����
	static unsigned char temp[4],t[4];
	int i,j,k;
	for(i=0; i<4; i++) {
		memcpy(w[i],&key[i*4],4);
	}
	for(i=4; i<44; i++) {
		memcpy(temp,w[i-1],4);
		if(i%4==0) {
			memcpy(t,temp,1);
			memcpy(temp,&temp[1],3);
			memcpy(&temp[3],t,1);
			for(j=0,k=(i/4)*4; j<4; j++,k++) {
				temp[j]=SBox_Chg(temp[j],8)^RC[k-4];
			}
			for(j=0; j<4; j++) {
				w[i][j]=w[i-4][j]^temp[j];
			}
		} else
			for(j=0; j<4; j++) {
				w[i][j]=w[i-4][j]^temp[j];
			}
	}
}

void rows_mov(unsigned char *sin) {  //����λ����
	static unsigned char temp[4];
	memcpy(temp,&sin[4],1);     //��һ�в��䣬�ڶ���ѭ������һλ
	memcpy(&sin[4],&sin[5],3);
	memcpy(&sin[7],temp,1);

	memcpy(temp,&sin[8],2);      //������ѭ��������λ
	memcpy(&sin[8],&sin[10],2);
	memcpy(&sin[10],temp,2);

	memcpy(temp,&sin[12],3);     //������ѭ��������λ
	memcpy(&sin[12],&sin[15],1);
	memcpy(&sin[13],temp,3);
}

void rows_mov_1(unsigned char *sin) {  //����λ����������
	static unsigned char temp[4];
	memcpy(temp,&sin[7],1);
	memcpy(&sin[5],&sin[4],3);
	memcpy(&sin[4],temp,1);       //ѭ������1λ��ʵ��Ϊ�ڴ������໥����

	memcpy(temp,&sin[10],2);
	memcpy(&sin[10],&sin[8],2);
	memcpy(&sin[8],temp,2);      //ѭ������2λ��ʵ��Ϊ�ڴ������໥����

	memcpy(temp,&sin[12],1);
	memcpy(&sin[12],&sin[13],3);
	memcpy(&sin[15],temp,1);      //ѭ������3λ��ʵ��Ϊ�ڴ������໥����
}

unsigned char GF2mul(unsigned char a, unsigned char b) {
//������GF(2^8)�ϵĳ˷�
	unsigned char bw[4];
	unsigned char res=0;
	int i;
	bw[0] = b;
	for(i=1; i<4; i++) {    //ѭ�����Σ��õ���2��4��8���ֵ
		bw[i] = bw[i-1]<<1;    //ԭ��ֵ��2
		if(bw[i-1]&0x80) {  //�ж�ԭ��ֵ�Ƿ�С��0x80
			bw[i]^=0x1b;    //����Ϊ1����ȥ0x1b
		}
	}
	for(i=0; i<4; i++) {
		if((a>>i)&0x01) {    //������a��ֵ��ʾΪ1��2��4��8���������
			res ^= bw[i];
		}
	}
	return res;
}

void columnsmix(unsigned char *sin) {
//�л�ϱ任
	unsigned char t[4][4]= {0};
	int i,j,k;
	for(i=0; i<4; i++)
		for(k=0; k<4; k++)
			for(j=0; j<4; j++)
				t[i][k]^=GF2mul(L_mix[i][j],sin[j*4+k]);
	memcpy(sin,t,16);
}


void columnsmix_1(unsigned char *sin) {
//���л�ϱ任
	unsigned char t[4][4]= {0};
	int i,j,k;
	for(i=0; i<4; i++)
		for(k=0; k<4; k++)
			for(j=0; j<4; j++)
				t[i][k]^=GF2mul(L_mix_1[i][j],sin[j*4+k]);
	memcpy(sin,t,16);
}

void addroundkey_start(unsigned char *sin,unsigned char (*p)[4]) {
//���һ������Կ�Ӳ���
	unsigned char out[16];
	int i,j,k=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++) {
			out[j*4+i]=sin[k]^p[i][j];
			k++;
		}
	memcpy(sin,out,16);
}


void addroundkey(unsigned char *sin,unsigned char (*p)[4]) {
//����Կ�Ӳ���
	unsigned char out[16];
	int i,j,k=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++) {
			out[j*4+i]=sin[j*4+i]^p[i][j];
			k++;
		}
	memcpy(sin,out,16);
}

void AES_Cry(unsigned char *sin) {
	int i,j,k=0;

	addroundkey_start(sin,&w[0]);
	for(i=1; i<10; i++) {
		for(j=0; j<16; j++)
			sin[j]=SBox_Chg(sin[j],8);
		rows_mov(sin);
		columnsmix(sin);
		addroundkey(sin,&w[i*4]);

	}
	for(j=0; j<16; j++)  //��ʮ���ּ���
		sin[j]=SBox_Chg(sin[j],8);
	rows_mov(sin);
	addroundkey(sin,&w[i*4]);
}

void AES_Dec(unsigned char *sin) {
	int i,j,k=0;

	addroundkey(sin,&w[40]);
	for(i=9; i>0; i--) {
		rows_mov_1(sin);
		for(j=0; j<16; j++)
			sin[j]=SBox_1_Chg(sin[j],8);
		addroundkey(sin,&w[i*4]);
		columnsmix_1(sin);
	}
	rows_mov_1(sin);
	for(j=0; j<16; j++)
		sin[j]=SBox_1_Chg(sin[j],8);
	addroundkey(sin,&w[i*4]);
}

int main() {
	int i,j;

	unsigned char messages[17]= {0};
	unsigned char Mykey[17]= {0};
	unsigned char Yourkey[17]= {0};
	printf("Please Input message:(128bit)\n");
	gets((char*)messages);

	printf("Please Input Key:(128bit)\n");
	gets((char*)Mykey);

	Getkey(Mykey);
	AES_Cry(messages);
	printf("\n\n���ܺ�");
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			printf("%x ",messages[j*4+i]);
	printf("\nPlease Input Key:(128bit)\n");
	gets((char*)Yourkey);
	Getkey(Yourkey);
	AES_Dec(messages);
	printf("\n���ܺ�");
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			printf("%c ",messages[j*4+i]);
	printf("\n");
}
