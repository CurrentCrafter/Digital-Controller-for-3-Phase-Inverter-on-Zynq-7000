#ifndef GUI_H
#define GUI_H


u8 GUI_Find_Command(const char *str, const char *str2);
void GUI_UART_Flush(void);

void GUI_Send_State(void);
void GUI_Send_locked(void);
void GUI_Send_feat1(void);
void GUI_Send_feat2(void);
void GUI_send_vars(void);
void GUI_Send_Last_I_Values(uint16_t index);
void GUI_Send_Last_U_Values(uint16_t index);
void GUI_Send_Last_DQ_Values(uint16_t index);
void GUI_Send_Temperature(void);
void GUI_Process_Commands(void);

void GUI_Send_Sogi(void);
void GUI_Send_Sogi_Debug(uint16_t index);
void GUI_Master(void); //Example Function for future Use

void GUI_Execute_Command(u8* Command);


#endif // GUI_H
