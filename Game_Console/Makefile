# Derleyici ve bayraklar
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Hedef dosya adı
TARGET = game

# Kaynak dosyalar
SRC = game.c

# Varsayılan hedef
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Temizleme komutu
clean:
	rm -f $(TARGET)

