#ifndef UXP1A_TEST_UTILS_H
#define UXP1A_TEST_UTILS_H


unsigned long hash(unsigned char *str);
void write_shm_id_to_file(const char *filename, int shmId);
int read_shm_id_from_file(const char *filename);

#endif //UXP1A_TEST_UTILS_H
