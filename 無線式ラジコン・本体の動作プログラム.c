/*
 * File:   receiver.c
 * Author: TDS001
 *
 * Created on December 13, 2024, 11:59 PM
 */
#include "PIC18F23K20.h"
#define _XTAL_FREQ 16000000

//MoterDriver���͗p
#define Ain2 PORTBbits.RB0
#define Ain1 PORTBbits.RB1
#define STBY PORTBbits.RB2
#define Bin1 PORTBbits.RB3
#define Bin2 PORTBbits.RB4
#define APWM PORTCbits.RC1
#define BPWM PORTCbits.RC2

//�ϐ�
char direction;

void main(void) {
    
    //���C���N���b�N�̐ݒ�
    OSCCONbits.IRCF=0b111;//16MHz
    
    //���o�͐ݒ�
    //�o�̓��[�h
    TRISBbits.RB0=0;
    TRISBbits.RB1=0;
    TRISBbits.RB2=0;
    TRISBbits.RB3=0;
    TRISBbits.RB4=0;
    
    //PWM�p��CCP1,CCP2���o�͂ɐݒ�
    TRISCbits.RC1=0;
    TRISCbits.RC2=0;
    
    
    //�����ݒ�
    STBY=1;//���[�^�[�h���C�o�[��STBY�s����ON�ɂ��Ă���
    
    //PWM�ݒ�
    //CCP1,CCP2��PWM���[�h�ɐݒ�
    CCP1CONbits.CCP1M=0b1100;
    CCP2CONbits.CCP2M=0b1100;
    
    CCPR1L=0b10000000;//CCP1�s����PWM�o��100%
    CCPR2L=0b10000000;//CCP2�s����PWM�o��50%
    
    
    //�^�C�}2���g�p�����f���[�e�B�T�C�N���̐ݒ�
	//�^�C�}2��CCP1,CCCP2�Ƃ̊����Ńf���[�e�B�T�C�N���̔䗦�����߂�
    T2CONbits.TMR2ON=1;//�ݒ�̂��߂Ƀ^�C�}2���N��
    T2CONbits.T2OUTPS0=1;//�|�X�g�X�P�[���[1�F1
    T2CONbits.T2CKPS0=0b00;//�v���X�P�[���[��1�F1
    PR2=255;
    
    INTCONbits.PEIE=1;//���ӊ��荞�݂̋���
    INTCONbits.GIE=1;//�S�̊��荞�݂̋���
    
    
    T2CONbits.TMR2ON=0;//�ݒ�̏I���Ƀ^�C�}2���~�߂�
    
    
    
    //UART�ݒ�
    TRISCbits.RC6=0;//TX
    TRISCbits.RC7=1;//RX
    
    SPBRG=8;//16MHz�̎��A115.2kbps
    
    //UART�ݒ�//�񓯊����[�h�ł̐ݒ�
    TXSTAbits.SYNC=0;//�񓯊����[�h
    TXSTAbits.BRGH=1;//�����{�[���[�g
    BAUDCONbits.BRG16=0;//8bit�{�[���[�g�W�F�l���[�^
    
    
    //UART�ݒ�//��M�̐ݒ�
    TXSTAbits.TXEN=1;//���M�@�̗L����//�A�����M����
    RCSTAbits.CREN=1;//��M�@�̗L����//�A����M����
    RCSTAbits.SPEN=1;//�V���A���|�[�g�̗L����
    
    //�����ݐݒ�
    
    PIE1bits.TMR2IE=1;//�^�C�}2�����ݐݒ�
    PIE1bits.RCIE=1;//UART��M�����݂̋���
    INTCONbits.PEIE=1;//���ӊ��荞�݂̋���
    INTCONbits.GIE=1;//�S�̊��荞�݂̋���
    
    T2CONbits.TMR2ON=1; //�^�C�}2�X�^�[�g
   
    while(1){
  
    }
    
     
    return;
}


//�O�������݊֐�
//UART��M
void __interrupt() isr(void){
    if(PIR1bits.RCIF==1){//�R���g���[���[������̏�����M������
        direction=RCREG;//��M�f�[�^��ϐ�direction�ɓ����
    }
    
    if(direction=='L'){//��M�����f�[�^��'L'�̎�
        /*�A�i���O�X�e�B�b�N�̍�����50�ȏ�=L�̏ꍇ�͍��ɋȂ���
        �����̃��[�^�[�̉�]���x�𗎂Ƃ�*/
        CCPR1L=0b10000000;//CCP1�s����PWM�o��100%
        CCPR2L=0b11111111;//CCP2�s����PWM�o��30%
        Ain1=1, Ain2=0;
        Bin1=1, Bin2=0;
        APWM=1, BPWM=1;
    }
    else if(direction=='R'){//��M�����f�[�^��'R'�̎�
     /*�A�i���O�X�e�B�b�N�̉E����50�ȏ�=R�̏ꍇ�͉E�ɋȂ���
     �E���̃��[�^�[�̉�]���x�𗎂Ƃ�*/
        CCPR1L=0b11111111;//CCP1�s����PWM�o��100%
        CCPR2L=0b10000000;//CCP2�s����PWM�o��30%
        Ain1=1, Ain2=0;
        Bin1=1, Bin2=0;
        APWM=1, BPWM=1;
    }
    else if(direction=='U'){//��M�����f�[�^��'U'�̎�
        /*�A�i���O�X�e�B�b�N�̏����50�ȏ�=U�̏ꍇ�͒��i����
        ���E�̃��[�^�[�̉�]���x�͓���*/
        CCPR1L=0b11111111;//CCP1�s����PWM�o��100%
        CCPR2L=0b11111111;//CCP2�s����PWM�o��100%
        Ain1=1, Ain2=0;
        Bin1=1, Bin2=0;
        APWM=1, BPWM=1;
    }
    else if(direction=='D'){//��M�����f�[�^��'D'�̎�
     /*�A�i���O�X�e�B�b�N�̉�����50�ȏ�=D�̏ꍇ�̓o�b�N����
     ���E�̃��[�^�[�̉�]���x�͓���������]�����͋t����*/
        
        CCPR1L=0b11111111;//CCP1�s����PWM�o��100%
        CCPR2L=0b11111111;//CCP2�s����PWM�o��100% 
        Ain1=0, Ain2=1;
        Bin1=0, Bin2=1;
        APWM=1, BPWM=1;
    }
    else{
    /*�A�i���O�X�e�B�b�N�̓��͂��Ȃ���Ԃ̎��A���邢��
    ���r���[�ȓ��͂̎��̓X�g�b�v*/
        Ain1=0, Ain2=0;
        Bin1=0, Bin2=0;
        BPWM=1, APWM=1;
    }
}
          