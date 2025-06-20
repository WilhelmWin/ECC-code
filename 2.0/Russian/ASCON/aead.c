#include "ascon.h"
#include "word.h"
#include "constants.h"
// ========================================================================
// Функция AEAD шифрования для ASCON-128a
// ========================================================================

int crypto_aead_encrypt(
  uint8_t *c,               // Выходной зашифрованный текст (ciphertext)
  uint64_t *clen,           // Длина шифротекста (выход)
  const uint8_t *m,         // Входное сообщение (plaintext)
  uint64_t mlen,            // Длина входного сообщения
  const uint8_t *npub,      // Публичный nonce
  const uint8_t *k          // Ключ (секретный ключ для шифрования)
){

  // =====================================================================
  // Установка размера шифротекста
  // =====================================================================

  *clen = mlen + CRYPTO_ABYTES;  // Размер шифротекста равен размеру
                                 // сообщения + тег

  // =====================================================================
  // Загрузка ключа и nonce
  // =====================================================================
  const uint64_t K0 = LOADBYTES(k, 8);  // Загружаем первую часть ключа
  const uint64_t K1 = LOADBYTES(k + 8, 8);  // Загружаем вторую часть ключа
  const uint64_t N0 = LOADBYTES(npub, 8);  // Загружаем первую часть nonce
  const uint64_t N1 = LOADBYTES(npub + 8, 8);  // Загружаем вторую часть
                                               // nonce

  // =====================================================================
  // Инициализация состояния ASCON
  // =====================================================================
  ascon_state_t s;
  s.x[0] = ASCON_128A_IV;  // Устанавливаем начальное значение для
                           // состояния
  s.x[1] = K0;  // Устанавливаем первую часть ключа
  s.x[2] = K1;  // Устанавливаем вторую часть ключа
  s.x[3] = N0;  // Устанавливаем первую часть nonce
  s.x[4] = N1;  // Устанавливаем вторую часть nonce
  P12(&s);  // Применяем 12 раундов перестановки
  s.x[3] ^= K0;  // XOR с первой частью ключа
  s.x[4] ^= K1;  // XOR со второй частью ключа

  // =====================================================================
  // Применение разделения доменов
  // =====================================================================
  s.x[4] ^= DSEP();  // XOR с константой разделения доменов

  // =====================================================================
  // Обработка полных блоков сообщения
  // =====================================================================
  while (mlen >= ASCON_128A_RATE) {
    s.x[0] ^= LOADBYTES(m, 8);  // XOR с блоком сообщения
    s.x[1] ^= LOADBYTES(m + 8, 8);  // XOR с следующим блоком сообщения
    STOREBYTES(c, s.x[0], 8);  // Сохраняем блок шифротекста
    STOREBYTES(c + 8, s.x[1], 8);  // Сохраняем второй блок шифротекста
    P8(&s);  // Применяем 8 раундов перестановки для сообщения
    m += ASCON_128A_RATE;  // Переходим к следующему блоку сообщения
    c += ASCON_128A_RATE;  // Переходим к следующему блоку шифротекста
    mlen -= ASCON_128A_RATE;  // Уменьшаем оставшуюся длину сообщения
  }

  // =====================================================================
  // Обработка последнего блока сообщения
  // =====================================================================
  if (mlen >= 8) {
    s.x[0] ^= LOADBYTES(m, 8);  // XOR с последним блоком сообщения
    s.x[1] ^= LOADBYTES(m + 8, mlen - 8);  // XOR с неполным блоком
    STOREBYTES(c, s.x[0], 8);  // Сохраняем блок шифротекста
    STOREBYTES(c + 8, s.x[1], mlen - 8);  // Сохраняем неполный блок
                                          // шифротекста
    s.x[1] ^= PAD(mlen - 8);  // Применяем паддинг, если необходимо
  } else {
    s.x[0] ^= LOADBYTES(m, mlen);  // XOR с последней частью сообщения
    STOREBYTES(c, s.x[0], mlen);  // Сохраняем шифротекст
    s.x[0] ^= PAD(mlen);  // Применяем паддинг
  }
  m += mlen;
  c += mlen;

  // =====================================================================
  // Завершение шифрования
  // =====================================================================
  s.x[2] ^= K0;  // XOR с первой частью ключа
  s.x[3] ^= K1;  // XOR со второй частью ключа
  P12(&s);  // Применяем 12 раундов перестановки
  s.x[3] ^= K0;  // XOR с первой частью ключа снова
  s.x[4] ^= K1;  // XOR со второй частью ключа снова

  // =====================================================================
  // Сохраняем тег и выводим байты
  // =====================================================================
  STOREBYTES(c, s.x[3], 8);  // Сохраняем тег
  STOREBYTES(c + 8, s.x[4], 8);  // Сохраняем вторую часть тега

  return 0;  // Возвращаем успех
}

// ========================================================================
// Функция расшифровки для AEAD с использованием ASCON-128a.
// ========================================================================
int crypto_aead_decrypt(
  uint8_t *m,              // Выходное сообщение
  uint64_t *mlen,          // Длина расшифрованного сообщения (выход)
  uint8_t *nsec,           // Секретный nonce
  const uint8_t *c,        // Входной шифротекст (зашифрованное сообщение)
  uint64_t clen,           // Длина шифротекста
  const uint8_t *npub,     // Публичный nonce
  const uint8_t *k         // Ключ (тот же ключ, что и при шифровании)
){
  (void)nsec;  // 'nsec' не используется, подавление предупреждения

  // Проверяем, что длина шифротекста не меньше размера тега
  if (clen < CRYPTO_ABYTES) return -1;

  // =====================================================================
  // Устанавливаем размер расшифрованного сообщения (m) путем вычитания
  // размера тега (CRYPTO_ABYTES) из длины шифротекста.
  // =====================================================================
  *mlen = clen - CRYPTO_ABYTES;

  // =====================================================================
  // Загружаем ключ и nonce из предоставленных буферов в 64-битные
  // переменные для эффективной обработки.
  // =====================================================================
  const uint64_t K0 = LOADBYTES(k, 8);      // Загружаем первую часть ключа
  const uint64_t K1 = LOADBYTES(k + 8, 8);  // Загружаем вторую часть ключа
  const uint64_t N0 = LOADBYTES(npub, 8);   // Загружаем первую часть nonce
  const uint64_t N1 = LOADBYTES(npub + 8, 8);// Загружаем вторую часть nonce

  // =====================================================================
  // Инициализируем состояние ASCON с использованием IV (инициализационного
  // вектора), ключа и значений nonce.
  // =====================================================================
  ascon_state_t s;
  s.x[0] = ASCON_128A_IV;  // Устанавливаем начальное состояние (IV)
  s.x[1] = K0;             // Устанавливаем первую часть ключа
  s.x[2] = K1;             // Устанавливаем вторую часть ключа
  s.x[3] = N0;             // Устанавливаем первую часть nonce
  s.x[4] = N1;             // Устанавливаем вторую часть nonce

  P12(&s);  // Выполняем перестановку
  s.x[3] ^= K0;  // XOR с ключом K0
  s.x[4] ^= K1;  // XOR с ключом K1

  // =====================================================================
  // Выполняем разделение доменов для различения операций шифрования
  // и расшифровки.
  // =====================================================================
  s.x[4] ^= DSEP();

  // =====================================================================
  // Обрабатываем полные блоки шифротекста. Шифротекст XOR'ится с
  // состоянием для получения исходного сообщения.
  // =====================================================================
  clen -= CRYPTO_ABYTES;  // Корректируем длину шифротекста, вычитая тег
  while (clen >= ASCON_128A_RATE) {
    uint64_t c0 = LOADBYTES(c, 8);
    uint64_t c1 = LOADBYTES(c + 8, 8);
    STOREBYTES(m, s.x[0] ^ c0, 8);  // XOR с состоянием для получения
                                    // исходного сообщения
    STOREBYTES(m + 8, s.x[1] ^ c1, 8);
    s.x[0] = c0;  // Обновляем состояние с шифротекстом
    s.x[1] = c1;
    P8(&s);  // Выполняем перестановку
    m += ASCON_128A_RATE;
    c += ASCON_128A_RATE;
    clen -= ASCON_128A_RATE;
  }

  // =====================================================================
  // Обработка последнего (неполного) блока шифротекста.
  // Обрабатываем паддинг соответственно.
  // =====================================================================
  if (clen >= 8) {
    uint64_t c0 = LOADBYTES(c, 8);
    uint64_t c1 = LOADBYTES(c + 8, clen - 8);
    STOREBYTES(m, s.x[0] ^ c0, 8);
    STOREBYTES(m + 8, s.x[1] ^ c1, clen - 8);
    s.x[0] = c0;
    s.x[1] = CLEARBYTES(s.x[1], clen - 8);
    s.x[1] |= c1;
    s.x[1] ^= PAD(clen - 8);
  } else {
    uint64_t c0 = LOADBYTES(c, clen);
    STOREBYTES(m, s.x[0] ^ c0, clen);
    s.x[0] = CLEARBYTES(s.x[0], clen);
    s.x[0] |= c0;
    s.x[0] ^= PAD(clen);
  }
  m += clen;
  c += clen;

  // =====================================================================
  // Завершаем расшифровку, выполняем обратный XOR с ключами
  // и последнюю перестановку.
  // =====================================================================
  s.x[2] ^= K0;
  s.x[3] ^= K1;

  P12(&s);
  s.x[3] ^= K0;
  s.x[4] ^= K1;

  // =====================================================================
  // Вычисляем тег и проверяем, был ли успешным процесс расшифровки.
  // Если тег не совпадает, возвращаем ошибку.
  // =====================================================================
  uint8_t t[16];
  STOREBYTES(t, s.x[3], 8);
  STOREBYTES(t + 8, s.x[4], 8);

  // =====================================================================
  // Проверка тега в постоянное время, чтобы предотвратить тайминговые
  // атаки. Результат будет 0, если тег совпадает, и ненулевым,
  // если он не совпадает.
  // =====================================================================
  int i;
  int result = 0;
  for (i = 0; i < CRYPTO_ABYTES; ++i) result |= c[i] ^ t[i];
  result = (((result - 1) >> 8) & 1) - 1;

  // =====================================================================
  // Отладка: вывод расшифрованного сообщения (m).
  // =====================================================================

  return result;  // Возвращаем 0, если расшифровка успешна, иначе ошибка
}
