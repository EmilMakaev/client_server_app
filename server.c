#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "erproc.h"

// man htons man listen man 2 accept
int main()
{
  // AF_INET - ipv4
  // SOCK_STREAM - TCP, SOCK_DGRAM - UDP
  int server = Socket(AF_INET, SOCK_STREAM, 0);

  // for protocol ipv4
  struct sockaddr_in adr = {0}; // 0.0.0.0
  adr.sin_family = AF_INET;
  adr.sin_port = htons(34543);

  Bind(server, (struct sockaddr *)&adr, sizeof adr);

  // до 5 клиентов может ожидать в очереди
  Listen(server, 5);

  socklen_t adrlen = sizeof adr;
  // переменная adr содержит информацию о клиенте
  // Accept вернет другой сокет (файловый дескриптор) (descriptor for the accepted socket)
  // через который можно общаться с нашим клиентом
  int fd = Accept(server, (struct sockaddr *)&adr, &adrlen);

  // read message that client send to server
  // read вернет количество считаных байт
  ssize_t nread;
  // buf 256 ячеек типа chat bytes
  char buf[256];
  // указываем что мы не можем принять больше 256 bytes, таким образом не будет переполнения
  // и ошибок на подобие segmentation fault
  nread = read(fd, buf, 256);

  if (nread == -1)
  {
    perror("Read failed");
    exit(EXIT_FAILURE);
  }

  // nread == 0 клиент говорит нам что больше не собирается нам что-то передовать, но
  // это не говорит нам что клиент не собирается больше принимать от нас сообщения
  if (nread == 0)
  {
    printf("END OF FILE occurred\n");
  }

  // печатаем в консоли, то что получили
  write(STDOUT_FILENO, buf, nread);

  // передаем ответ клиенту
  write(fd, buf, nread);

  // pending 15 second
  sleep(15);

  // закрываем сокет с клиентом и прослушивающий сокет
  close(fd);
  close(server);

  return 0;
}

// RUN SERVER
// -W -Wall -Werror запуск со всеми возможными предупреждениями
// -std=c99 стандарт языка С от 99 года.
// конечный файл будет называться server
// gcc server.c erproc.c -W -Wall -Werror -std=c99 -o server && ./server
