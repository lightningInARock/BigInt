#ifndef __BIG_INT__
#define __BIG_INT__

#include <iostream>
#include <string>
#include <exception>

class BigInt {
	// Defining word size to get maximum memory efficency
	static constexpr const char* uint32_max = "4294967295";
	static constexpr const char* uint64_max = "18446744073709551615";
	static const size_t uint32_m = 4294967295;
	static const size_t uint64_m = 18446744073709551615ull;
	static constexpr const char* max = 
		(sizeof(size_t) == 8)? uint64_max: uint32_max;
	static constexpr const size_t intmax =
		(sizeof(size_t) == 8)? uint64_m: uint32_m;
	static constexpr const unsigned int chunk_size = sizeof(size_t) * 8;

	// Better do it yourself
	static void reverse(std::string &s);

	// +1 to number represented as string
	static void inc(std::string &s);

	static void inv(std::string &s);
	
	// Need divides to get binary
	static BigInt div_by_2(const BigInt &b);

	static std::string div_by_2(const std::string &num);
	
	// Need multiplies to get decimal
	static BigInt mul_by_2(const BigInt &b);

	static std::string mul_by_2(const std::string &num);

	// Get binary representation of a number given as string
	static std::string bin(std::string num);

	// Add two big integers
	static std::string add(std::string num1, std::string num2);
	
	// Data stored in Little Endian
	size_t *_base;

	// How many size_t blocks (chunks)
	unsigned char _size;

public:
	
	class NonDigitCharException : public std::exception {
		const char* _msg;
		public:
			NonDigitCharException();

			NonDigitCharException(const char* message);
			
			const char* what();
	};

	BigInt();

	BigInt(const BigInt& b);
	
	BigInt(const std::string &num);
	
	BigInt(const std::string &&num);

	// Getter for _size
	unsigned int size();

	// Inc and Inv are used to get negative number
	// Increase BigInt by 1
	void inc();
	
	// Invert BigInt
	void inv();

	std::string to_bin_string() const;

	std::string to_string() const;
	
	BigInt operator-() const;
	
	BigInt operator+(const BigInt& b) const;

	BigInt operator-(const BigInt& b) const;

	BigInt& operator=(const BigInt& b);

	BigInt& operator=(const std::string& num);

	BigInt& operator=(const std::string&& num);

	~BigInt();
};

std::ostream& operator<<(std::ostream &os, const BigInt &b); 

#endif
