#pragma once

#include <stdint.h>

// ��������
enum ZZTYPE
{
	ZZT_INT,    //int32_t
	ZZT_BIGINT, //int64_t
	ZZT_BOOL,   //int8_t
	ZZT_STRING, //tbrow_desc.size
	ZZT_FLOAT,  //float
	ZZT_DOUBLE, //double
};

// �������Ͷ���

typedef int32_t zz_int;
typedef int64_t zz_bigint;
typedef int8_t  zz_bool;
typedef float   zz_float;
typedef double  zz_double;