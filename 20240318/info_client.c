#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]) 
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    // Nhap du lieu tu ban phim
    int mssv;
    char hoten[64];
    unsigned char ngay, thang;
    unsigned short nam;
    float diemtb;

    printf("Nhap MSSV: ");
    scanf("%d", &mssv);
    printf("Nhap ho ten: ");
    getchar();
    fgets(hoten, sizeof(hoten), stdin);
    hoten[strlen(hoten) - 1] = 0;
    
    printf("Nhap ngay, thang, nam sinh: ");
    scanf("%hhu%hhu%hu", &ngay, &thang, &nam);
    printf("Nhap diem trung binh: ");
    scanf("%f", &diemtb);

    // Dong goi du lieu
    char buf[256];
    int pos = 0;

    memcpy(buf, &mssv, sizeof(mssv));
    pos += sizeof(mssv);
    buf[pos++] = ngay;
    buf[pos++] = thang;
    memcpy(buf + pos, &nam, sizeof(nam));
    pos += sizeof(nam);
    memcpy(buf + pos, &diemtb, sizeof(diemtb));
    pos += sizeof(diemtb);
    memcpy(buf + pos, hoten, strlen(hoten));
    pos += strlen(hoten);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    // Gui sang server
    send(client, buf, pos, 0);

    close(client);

    return 0;
}