# 📄 Документация к системе сборки
## 🔍 Описание

Данный Makefile определяет правила для компиляции и компоновки клиент-серверного криптографического приложения. Он компилирует криптографические модули ECC и ASCON, собирает их в статические библиотеки и линкует в исполняемые файлы client и server.
## 🔧 Компилятор и флаги
```make
CC = gcc
CFLAGS = -Wall -Wextra -O2 -fstack-protector-strong -fPIE -D_FORTIFY_SOURCE=2
LDFLAGS = -pie -Wl,-z,relro -Wl,-z,now
```
- `CC`: используемый C-компилятор (GCC).
- `CFLAGS`: флаги компиляции для предупреждений, оптимизации и безопасности:
  - `Wall -Wextra`: включение стандартных и дополнительных предупреждений.
  - `O2`: оптимизация второго уровня.
  - `fstack`-protector-strong: защита стека от переполнения.
  - `fPIE` и `-pie`: позиционно-независимые исполняемые файлы (для ASLR).
  - `D_FORTIFY_SOURCE=2`: проверки во время компиляции и выполнения.

## 🎯 Названия целевых исполняемых файлов
```make
SERVER_TARGET = server
CLIENT_TARGET = client
```
- Определяет имена итоговых исполняемых файлов.
## 📁 Конфигурация каталогов
```make
ASCON_DIR = ASCON
```
- Код для криптографии ASCON находится в каталоге ASCON.
## 🧩 Исходные и объектные файлы
### ECC
```make
ECC_SRC = ECC.c
ECC_OBJ = ECC.o
```
- Один исходный файл для ECC.

- Компилируется в один объектный файл.

### ASCON
```make
ASCON_SRC = $(ASCON_DIR)/aead.c
ASCON_OBJ = $(ASCON_SRC:.c=.o)
```
- Исходник для шифрования/дешифрования ASCON.
- Путь к объектному файлу создаётся автоматически.

### Server / Client
```make
SERVER_SRC = server.c session.c drng.c error.c
CLIENT_SRC = client.c session.c drng.c error.c
```
- Server использует: `server.c`, общий `session.c`, ГСЧ `drng.c` и ошибки `error.c`.

- Client использует: `client.c`, `session.c` и `drng.c`, `error.c`.
```make
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
COMMON_OBJ = session.o drng.o error.o
```
- Объектные файлы создаются автоматически на основе исходников.

## 📚 Статические библиотеки
```make
LIBECC = libecc.a
LIBASCON = libascon.a
LIBRARIES = $(LIBECC) $(LIBASCON)
```
- Итоговые статические библиотеки из скомпилированных объектных файлов.

- Используются при линковке конечных исполняемых файлов.

## 🧷 Платформозависимая конфигурация
```make
ifeq ($(OS), Windows_NT)
LDFLAGS = -lws2_32
RM = del /f /q
NULL = nul
else
LDFLAGS =
RM = rm -f
NULL = /dev/null
endif
```
Учитывает различия между ОС:

- Линковочные флаги (в Windows нужен Winsock ws2_32).

- Команды для удаления файлов (rm против del).

- Устройства перенаправления в "ничто" (nul vs /dev/null).

## 🛠 Цель по умолчанию
```make
all: $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET) postbuild
```
Обеспечивает:

- Компиляцию статических библиотек.

- Линковку исполняемых файлов клиента и сервера.

- Очистку временных файлов после сборки.

## 📦 Сборка статических библиотек
```make
$(LIBECC): $(ECC_OBJ)
ar rcs $@ $^

$(LIBASCON): $(ASCON_OBJ)
ar rcs $@ $^
```
- `ar rcs`: архивирует объектные файлы в .a (статические библиотеки).

## 🔗 Линковка исполняемых файлов
```make
$(SERVER_TARGET): $(SERVER_OBJ) $(LIBRARIES)
$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJ) $(LIBRARIES)
$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
```
- Линкует каждый исполняемый файл с его объектами и нужными библиотеками.

## 🧱 Правило компиляции объектных файлов
```make
%.o: %.c
$(CC) $(CFLAGS) -c $< -o $@
```
- Шаблонное правило: компилирует `.c` в `.o`.
## 🧼 Очистка после сборки
```make
postbuild:
ifeq ($(OS), Windows_NT)
-$(RM) ECC.o session.o drng.o error.o
-$(RM) ASCON\\aead.o ASCON\\printstate.o
-$(RM) server.o client.o
-$(RM) $(LIBRARIES)
else
$(RM) $(ECC_OBJ) $(COMMON_OBJ) $(ASCON_OBJ) $(SERVER_OBJ)
$(RM) $(CLIENT_OBJ) $(LIBRARIES)
endif
```
- Удаляет временные и библиотечные файлы после успешной сборки.

- Использует разные команды в зависимости от ОС.

## 🧽 Команды очистки
### **`make clean`**
```make
clean:
ifeq ($(OS), Windows_NT)
-$(RM) ASCON\\aead.o ASCON\\printstate.o
-$(RM) $(LIBRARIES)
-$(RM) $(SERVER_TARGET) $(CLIENT_TARGET) *.exe
else
$(RM) $(ASCON_DIR)/*.o $(LIBRARIES) $(SERVER_TARGET) $(CLIENT_TARGET)
$(RM) $(ECC_OBJ) $(COMMON_OBJ)  $(SERVER_OBJ) $(CLIENT_OBJ)
endif
```
- Удаляет все объектные и исполняемые файлы.

### **`make distclean`**
```make
distclean:
ifeq ($(OS), Windows_NT)
-$(RM) *~ *.bak
else
$(RM) *~ *.bak
endif
```
- Удаляет резервные и временные файлы (например, *~, *.bak).