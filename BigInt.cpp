#include "BigInt.hpp"

void BigInt::reverse(std::string &s) {
	for(size_t i = 0; i < s.size() / 2; ++i) {
		char tmp = s[i];
		s[i] = s[s.size() - i - 1];
		s[s.size() - i - 1] = tmp;
	}
}

void BigInt::inc(std::string &s) {
	bool carry = 1;
	for(size_t i = s.size() - 1; i >= 0; --i) {
		s[i] += carry;
		if(s[i] == '2') {
			s[i] = '0';
			carry = 1;
		} else {
			// carry = 0;
			break;
		}
	}
}

void BigInt::inv(std::string &s) {
	// as '0' = 48
	// and '1' = 49
	for(size_t i = 0; i < s.size(); ++i) {
		s[i] ^= 1;
	}
}

BigInt BigInt::div_by_2(const BigInt &b) {
	BigInt res(b);
	
	size_t negative = b._base[b._size - 1] & (1ull << (chunk_size - 1));

	if(res._size == 1) {
		res._base[0] >>= 1;
		return res;
	}

	// Little Endian
	for(int i = 0; i < res._size - 1; ++i) {
		res._base[i] >>= 1;
		size_t bit = res._base[i + 1] & 1ull;
		res._base[i] |= bit << (chunk_size - 1);
	}
	res._base[b._size - 1] >>= 1;
	res._base[b._size - 1] |= negative;
	
	return res;
}

std::string BigInt::div_by_2(const std::string &num) {
	if(num == "1") {
		return "0";
	}

	std::string res;

	// Ignore initial zeros
	size_t i = 0;
	while(num[i] == '0') {
		++i;
	}

	char tmp = 0;
	for(; i < num.size(); ++i) {
		tmp += num[i] - '0';
		
		if(tmp < 2 && ((i + 1) != num.size())) {
			res.push_back('0');
			tmp *= 10;
			tmp += num[++i] - '0';
		}

		res.push_back(tmp / 2 + '0');
		tmp %= 2;
		tmp *= 10;
	}

	return res;
}

BigInt BigInt::mul_by_2(const BigInt& b) {
	BigInt res = b;

	bool bit = 0;
	for(unsigned char i = 0; i < res._size; ++i) {
		bool fbit = res._base[i] & (1ull << (chunk_size - 1));
		res._base[i] <<= 1;
		res._base[i] |= bit;
		bit = fbit;
	}

	// If we still have 1 bit to add
	// This also means that number was negative
	if(bit) {
		size_t* tmp = new size_t[res._size + 1];
		for(unsigned char i = 0; i < res._size; ++i) {
			tmp[i] = res._base[i];
		}
		tmp[res._size] = intmax;
		delete res._base;
		res._base = tmp;
	} else {
		// If number was positive
		// But after shifting high bit became 1
		if(res._base[res._size - 1] & (1ull << (chunk_size - 1))) {
			size_t* tmp = new size_t[res._size + 1];
			for(unsigned char i = 0; i < res._size; ++i) {
				tmp[i] = res._base[i];
			}
			tmp[res._size] = 0;
			delete res._base;
			res._base = tmp;
		}
	}

	return res;
}

std::string BigInt::mul_by_2(const std::string &num) {	
	return add(num, num);
}

std::string BigInt::bin(std::string num) {
	char sign = '0';
	if(num[0] == '-') {
		num = num.substr(1);
		sign = '1';
	}
   
	std::string res;
	while(num != "0") {
		bool c = (num[num.size() - 1] - '0') & 1;
		res.push_back(c + '0');
		num = div_by_2(num);
	}
	reverse(res);

	if(sign == '1') {
		for(size_t i = 0; i < res.size(); ++i) {
			res[i] ^= 1;
		}
		inc(res);
	}

	size_t diff = res.size() % chunk_size;

	if(diff) {
		std::string aligned(chunk_size - diff, sign);
		aligned.append(res);
		return aligned;
	} else {
		// If perfectly fitted, but it was positive number
		// and sign bit became 1
		if(res[0] != sign) {
			std::string aligned(chunk_size, sign);
			aligned.append(res);
			return aligned;
		}
	}

	return res;
}

std::string BigInt::add(std::string num1, std::string num2) {
	bool num1IsSmaller = num1.size() < num2.size();
	size_t size = num1IsSmaller? num1.size(): num2.size();
	std::string &res = num1IsSmaller? num2: num1;

	reverse(num1);
	reverse(num2);

	bool carry = 0;
	for(size_t i = 0; i < size; ++i) {
		char c = (num1[i] - '0') + (num2[i] - '0') + carry;
		if(c > 9) {
			carry = 1;
			c %= 10;
		} else {
			carry = 0;
		}
		res[i] = c + '0';
	}

	if(carry) {
		for(size_t i = size; i < res.size(); ++i) {
			if(res[i] == '9') {
				res[i] = '0';
			} else {
				++res[i];
				carry = 0;
				break;
			}
		}

		if(carry) {
			res.push_back('1');
		}
	}

	reverse(res);
	return res;
}

BigInt::NonDigitCharException::NonDigitCharException() : _msg(nullptr) {}

BigInt::NonDigitCharException::NonDigitCharException(const char* message) :
	_msg(message) {}

const char* BigInt::NonDigitCharException::what() {
	return _msg;
}

BigInt::BigInt() : _base(new size_t(0)), _size(1) {}

BigInt::BigInt(const BigInt& b) : _base(new size_t[b._size]),
   	_size(b._size)
{
	for(unsigned char i = 0; i < _size; ++i) {
		_base[i] = b._base[i];
	}	
}

BigInt::BigInt(const std::string &num) {
	if(num.size() == 0) {
		_size = 1;
		_base = new size_t(0);
		return;
	}	
	
	size_t i = (num[0] == '-')? 1: 0;
	for(; i < num.size(); ++i) {
		if(num[i] < '0' || num[i] > '9') {
			throw NonDigitCharException(
				"Given string contained non-digit character");
		}
	}

	std::string binary = bin(num);
	_size = (binary.size() % chunk_size)?
		binary.size() / chunk_size + 1:
		binary.size() / chunk_size;
	_base = new size_t[_size]();
	
	for(unsigned char i = 1; i <= _size; ++i) {
		for(unsigned char j = 0; j < chunk_size; ++j) {
			_base[i - 1] <<= 1;
			size_t bit = 
				binary[binary.size() - chunk_size * i + j] - '0';
			_base[i - 1] |= bit;
		}
	}
}

BigInt::BigInt(const std::string &&num) {
	if(num.size() == 0) {
		_size = 1;
		_base = new size_t(0);
		return;
	}	
	
	size_t i = (num[0] == '-')? 1: 0;
	for(; i < num.size(); ++i) {
		if(num[i] < '0' || num[i] > '9') {
			throw NonDigitCharException(
				"Given string contained non-digit character");
		}
	}

	std::string binary = bin(num);
	_size = (binary.size() % chunk_size)?
		binary.size() / chunk_size + 1:
		binary.size() / chunk_size;
	_base = new size_t[_size]();
	
	for(unsigned char i = 1; i <= _size; ++i) {
		for(unsigned char j = 0; j < chunk_size; ++j) {
			_base[i - 1] <<= 1;
			size_t bit = 
				binary[binary.size() - chunk_size * i + j] - '0';
			_base[i - 1] |= bit;
		}
	}
}

unsigned int BigInt::size() {
	return _size;
}

void BigInt::inc() {
	bool carry = 0;
	for(unsigned char i = 0; i < _size; ++i) {
		// if overflow
		size_t word = _base[i];
		_base[i] += carry + 1;
		if(word > intmax - carry - 1) {
			carry = 1;
		} else {
			// carry = 0;
			break;
		}
	}
	// Ignore last carry
	// -1 + 1 = 0
}

void BigInt::inv() {
	for(unsigned char i = 0; i < _size; ++i) {
		_base[i] = ~_base[i];
	}
}

std::string BigInt::to_bin_string() const {
	std::string res;
	bool negative = _base[_size - 1] & (1ull << (chunk_size - 1));
		
	// Little Endian
	for(unsigned char i = 0; i < _size; ++i) {
		std::string word(chunk_size, '0');
		for(unsigned char j = 0; j < chunk_size; ++j) {
			word[j] += (bool)(_base[i] & (1ull << j));
		}
		res.append(word);
	}

	reverse(res);
	return res;
}

std::string BigInt::to_string() const {
	bool negative = _base[_size - 1] & (1ull << (chunk_size - 1));
	if(negative) {
		const BigInt b = -(*this);
		std::string res = b.to_string();
		reverse(res);
		res.push_back('-');
		reverse(res);
		return res;
	}

	std::string res = "0";

	for(unsigned char i = 0; i < _size; ++i) {
		for(unsigned char j = 0; j < chunk_size; ++j) {
			if(_base[i] & (1ull << j)) {
				std::string tmp = "1";

				// Get 2^j
				for(unsigned char k = 0; k < i * chunk_size + j; ++k) {
					tmp = mul_by_2(tmp);
				}
				res = add(res, tmp);
			}
		}
	}

	return res;
}

BigInt BigInt::operator-() const {
	BigInt res = *this;
	res.inv();
	res.inc();
	return res;
}


BigInt BigInt::operator+(const BigInt& num) const {
	BigInt res = (_size > num._size)? *this: num;
	unsigned char min = (_size < num._size)? _size : num._size;
	bool carry = 0;
	for(unsigned char i = 0; i < min; ++i) {
		res._base[i] = _base[i] + num._base[i] + carry;
		
		if(_base[i] > intmax - num._base[i] - carry) {
			carry = 1;
		} else {
			carry = 0;
			// No break here because it is not inc
			// It is full addition
		}
	}

	// If carry set, inc remaining
	if(carry) {
		for(unsigned char i = min; i < res.size(); ++i) {
			++res._base[i];
			if(res._base[i]) {
				// Now it is inc so
				// break if no overflow
				break;
			}
		}
	}
	// Ignore last carry
	// -1 + 1 = 0
	return res;
}

BigInt BigInt::operator-(const BigInt& b) const {
	return *this + (-b);
}

BigInt& BigInt::operator=(const BigInt& b) {
	_size = b._size;
	if(_base != nullptr) {
		delete _base;
	}
	_base = new size_t[_size];
	for(unsigned char i = 0; i < _size; ++i) {
		_base[i] = b._base[i];
	}

	return *this;
}

BigInt& BigInt::operator=(const std::string& num) {
	BigInt b(num);
	*this = b;	
	return *this;
}

BigInt& BigInt::operator=(const std::string&& num) {
	return *this = num;
}

BigInt::~BigInt() {
	delete _base;	
}

std::ostream &operator<<(std::ostream &os, const BigInt &b) {
	os << b.to_string();
	return os;
}
