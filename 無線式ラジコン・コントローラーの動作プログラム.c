#include "PIC18F23K20.h"
#define _XTAL_FREQ 16000000

//MaxChar�֐�:�W���C�X�e�B�b�N���X�������������o���AU,D,R,L�ŕԂ�
char MaxChar(int L, int R, int U, int D)
{
	char max='L';
    int num=L;
    
	if(num<R){
		max='R';
        num=R;
	}
	
	if(num<U){
		max='U';
        num=U;
	}
	
	if(num<D){
		max='D';
        num=D;
	}
	
	return max;
}




void main(void)
{
	//���C���N���b�N�̐ݒ�
	OSCCONbits.IRCF=0b111;//16MHz
	
	//AD�ϊ��ݒ�
	TRISAbits.RA0=1;
	TRISAbits.RA1=1;
	ANSELbits.ANS0=1;//RA0���A�i���O���̓|�[�g�ɂ���
	ANSELbits.ANS1=1;//RA1���A�i���O���̓|�[�g�ɂ���
	
	ADCON2bits.ADCS=0b111;//��p�����I�V���[�^�̗��p
	ADCON1bits.VCFG=0b00;//�d�����t�@�����X��VDD�AVSS����Ƃ���
	ADCON2bits.ADFM=1;//���ʃt�H�[�}�b�g���E�l�߂ɂ���
	ADCON2bits.ACQT=0b101;//�A�N�C�W�V�������Ԃ�12TAD�ɐݒ�
	ADCON0bits.ADON=1;//ADC���W���[����ON
	
	//UART�ݒ�
	TRISCbits.RC6=1;//RX���͐ݒ�
	TRISCbits.RC7=1;//TX���͐ݒ�
    
     SPBRG=8;//16MHz�̎��A115.2kbps
	
	//UART�ݒ�ł̔񓯊����[�h�ł̐ݒ�
	TXSTAbits.SYNC=0;//�񓯊����[�h
	TXSTAbits.BRGH=1;//�����{�[���[�g
	BAUDCONbits.BRG16=0;//8bit�{�[���[�g�W�F�l���[�^
	
	//UART�ݒ�ł̑���M�̐ݒ�
	TXSTAbits.TXEN=1;//���M�@�̗L�����F�A�����M����
	RCSTAbits.SPEN=1;//�V���A���|�[�g�̗L����
	
    
	//�ϐ��̐錾
	unsigned int LR, UD;//�W���C�X�e�B�b�N�̍��E�A�㉺�̓��͒l���i�[
	unsigned int L,R,U,D;//�W���C�X�e�B�b�N�̓��͒l�����E�㉺��0�`100�ɕϊ��������̂��i�[
    char direction;
	
		
	while(1){
		//���E�̃f�[�^
		ADCON0bits.CHS=0b0000;//ADC���̓`�����l����AN0�ɐڑ�
		ADCON0bits.GO_nDONE=1;//AD�ϊ��J�n
		
		while(ADCON0bits.GO_nDONE==1){};//AD�ϊ����I���܂őҋ@
		LR=ADRES;//LR�ɃW���C�X�e�B�b�N�̍��E�̓��͒l���i�[����
		
		if(LR>=512){//�W���C�X�e�B�b�N���E�ɌX�����Ƃ�(�W���C�X�e�B�b�N�͖����͂���512�ɂȂ�)
			LR=LR-512;
			R=LR*100/512;//R�ɓ��͒l��0�`100�ɕϊ��������l���i�[
			L=0;//L��0���i�[����
		}
		else{
			LR=512-LR;
			L=LR*100/512;//L�ɓ��͒l��0�`100�ɕϊ��������l���i�[
			R=0;//R��0���i�[����
		}
		
		ADCON0bits.CHS=0b0001;//ADC���̓`�����l����AN1�ɐڑ�
		ADCON0bits.GO_nDONE=1;//AD�ϊ��J�n
		while(ADCON0bits.GO_nDONE==1){};//AD�ϊ����I���܂őҋ@
		UD=ADRES;//UD�ɃW���C�X�e�B�b�N�̏㉺�̓��͒l���i�[����
		
		if(UD>=512){//�W���C�X�e�B�b�N���E�ɌX�����Ƃ�(�W���C�X�e�B�b�N�͖����͂���512�ɂȂ�)
			UD=UD-512;
			U=UD*100/512;//U�ɓ��͒l��0�`100�ɕϊ��������l���i�[
			D=0;//D��0���i�[����
		}
		else{
			UD=512-UD;
			D=UD*100/512;//D�ɓ��͒l��0�`100�ɕϊ��������l���i�[
			U=0;//U��0���i�[����
		}
		
		//�W���C�X�e�B�b�N���X����������L,R,U,D�Ŏ��o��
		direction=MaxChar(L,R,U,D);
        
        if(direction=='L' && L>=50){//�W���C�X�e�B�b�N�������ɔ����ȏ�X������
             TXREG='L';//'L'�𑗐M
        }
        else if(direction=='R' && R>=50){//�W���C�X�e�B�b�N���E���ɔ����ȏ�X������
             TXREG='R';//'R'�𑗐M
        }
        else if(direction=='U' && U>=50){//�W���C�X�e�B�b�N���㑤�ɔ����ȏ�X������
             TXREG='U';//'U'�𑗐M
        }
        else if(direction=='D' && D>=50){//�W���C�X�e�B�b�N�������ɔ����ȏ�X������
             TXREG='D';//'D'�𑗐M
        }
        else{
            TXREG='S';//�W���C�X�e�B�b�N�𑀍삵�Ă��Ȃ����A�W���C�X�e�B�b�N�̓��͂��e�����ɔ��������̎���'S'�𑗐M
        }
       
		while(TXSTAbits.TRMT==0){}//���M�I���܂Ń|�[�����O
	}
	
	return;
}

