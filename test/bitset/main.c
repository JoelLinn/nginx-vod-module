#include <inttypes.h>
#include <stdio.h>
#include <ngx_core.h>
#include <vod/common.h>

#if (NGX_HAVE_VARIADIC_MACROS)

void
ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
    const char *fmt, ...)

#else

void
ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
    const char *fmt, va_list args)

#endif
{
}

#define assert(cond) if (!(cond)) { printf("Error: assertion failed, file=%s line=%d\n", __FILE__, __LINE__); success = FALSE; }

#define BITS (66)
bool_t test_bitset()
{
	bool_t success = TRUE;
	size_t mask[vod_array_length_for_bits(BITS)]; // [2]
	size_t mask2[vod_array_length_for_bits(BITS)];

	assert(sizeof(size_t) == 8); // run test on 64-bit system

	assert(vod_array_length_for_bits(63) == 1);
	assert(vod_array_length_for_bits(64) == 1);
	assert(vod_array_length_for_bits(65) == 2);

	vod_memzero(mask, sizeof(mask));

	assert(mask[0] == 0);
	assert(mask[1] == 0);
	for (int i = 0; i < BITS; i++)
	{
		assert(vod_is_bit_set(mask, i) == FALSE);
	}
	assert(vod_get_number_of_set_bits_in_mask(mask, BITS) == 0);
	assert(vod_is_all_bits_set(mask, BITS) == FALSE);
	assert(vod_is_any_bit_set(mask, BITS) == FALSE);

	vod_set_bit(mask, 0);
	assert(mask[0] == 1);
	assert(mask[1] == 0);
	assert(vod_get_number_of_set_bits_in_mask(mask, BITS) == 1);
	assert(vod_is_bit_set(mask, 0) == TRUE);
	assert(vod_is_any_bit_set(mask, 64) == TRUE);
	assert(vod_is_any_bit_set(mask, BITS) == TRUE);
	assert(vod_get_lowest_bit_set(mask, BITS) == 0);

	vod_set_bit(mask, 64);
	assert(mask[0] == 1);
	assert(mask[1] == 1);
	assert(vod_get_number_of_set_bits_in_mask(mask, BITS) == 2);
	assert(vod_get_lowest_bit_set(mask, BITS) == 0);

	vod_reset_bit(mask, 0);
	vod_reset_bit(mask, 64);
	assert(!vod_is_bit_set(mask, 0));
	assert(mask[0] == 0);
	assert(mask[1] == 0);

	vod_set_bits(mask, 64);
	assert(mask[0] == 0xffffffffffffffff);
	assert(mask[1] == 0);
	assert(vod_is_all_bits_set(mask, 64) == TRUE);
	assert(vod_is_all_bits_set(mask, BITS) == FALSE);
	vod_set_bit(mask, 64);
	vod_set_bit(mask, 65);
	assert(mask[0] == 0xffffffffffffffff);
	assert(mask[1] == 0b11);
	assert(vod_is_all_bits_set(mask, 66) == TRUE);

	vod_reset_bit(mask, 0);
	vod_reset_bit(mask, 1);
	assert(vod_get_number_of_set_bits_in_mask(mask, BITS) == 64);
	assert(vod_get_lowest_bit_set(mask, BITS) == 2);

	vod_memzero(mask, sizeof(mask));
	vod_set_bit(mask, 64);
	vod_set_bit(mask, 65);
	assert(vod_get_lowest_bit_set(mask, BITS) == 64);
	vod_reset_bit(mask, 64);
	assert(vod_get_lowest_bit_set(mask, BITS) == 65);

	vod_memzero(mask, sizeof(mask));

	vod_set_bit(mask, BITS + 2);
	vod_set_bits(mask, BITS);
	assert(mask[0] == 0xffffffffffffffff);
	assert(mask[1] == 0b10011);
	assert(vod_get_number_of_set_bits_in_mask(mask, BITS) == BITS);

	vod_reset_bits(mask, 64);
	assert(mask[0] == 0);
	assert(mask[1] == 0b10011);
	vod_reset_bits(mask, BITS);
	assert(mask[0] == 0);
	assert(mask[1] == 0b10000);

	vod_memzero(mask, sizeof(mask));
	vod_memzero(mask2, sizeof(mask));

	vod_and_bits(mask, mask, mask2, BITS);
	assert(vod_is_any_bit_set(mask, BITS) == FALSE);

	vod_set_bit(mask, 31);
	vod_and_bits(mask, mask, mask2, BITS);
	assert(vod_is_any_bit_set(mask, BITS) == FALSE);

	vod_set_bit(mask, 31);
	vod_set_bit(mask2, 31);
	vod_and_bits(mask, mask, mask2, BITS);
	assert(vod_is_any_bit_set(mask, BITS) == TRUE);
	assert(vod_is_bit_set(mask, 31) == TRUE);

	vod_memset(mask, 0x55, sizeof(mask));
	vod_memset(mask2, 0xff, sizeof(mask2));
	vod_and_bits(mask, mask, mask2, BITS);
	assert(mask[0] = 0x5555555555555555);
	assert(vod_get_number_of_set_bits_in_mask(mask, BITS) == BITS / 2);

	return success;
}

int main()
{
	if (!test_bitset())
	{
		printf("One or more tests failed.\n");
		return 1;
	}

	printf("All tests passed.\n");
	return 0;
}
