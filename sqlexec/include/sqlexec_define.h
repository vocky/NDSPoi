#ifndef SQLEXEC_DEFINE_H_
#define SQLEXEC_DEFINE_H_

typedef struct {
    int response_count;
} ST_Config;
typedef struct {
	int city_code;
	char match_info[64];
} ST_JsonRequest;
#endif /* SQLEXEC_DEFINE_H_ */
