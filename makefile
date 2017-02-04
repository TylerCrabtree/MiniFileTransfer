CC=gcc
COPTS=-g -std=c99 -Wall


All: mftpserve mftp

Server: mftpserve.c
	$(CC) $(COPTS) mftpserve.c -o mftpserve

Client: mftp.c
	$(CC) $(COPTS) mftp.c -o mftp