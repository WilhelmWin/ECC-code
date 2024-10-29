# Makefile для компиляции клиента и сервера

# Компилятор
CC = gcc

# Опции компилятора
CFLAGS = -Wall -g

# Имя исполняемых файлов
CLIENT = client
SERVER = server

# Исходные файлы
CLIENT_SRC = client_tuke.c
SERVER_SRC = server2.c

# Цели по умолчанию
all: $(CLIENT) $(SERVER)

# Правило для компиляции клиента
$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

# Правило для компиляции сервера
$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

# Очистка
clean:
	rm -f $(CLIENT) $(SERVER)

# Правило для запуска сервера и клиента в разных окнах терминала
run:
	@echo "Запуск сервера в новом окне..."
	xfce4-terminal -e "./$(SERVER) 8010; exec bash" &  # Запускаем сервер на порту 8010
	@echo "Запуск клиента в новом окне..."
	xfce4-terminal -e "./$(CLIENT) localhost 8010; exec bash"  # Запускаем клиента, подключаясь к серверу

.PHONY: all clean run
