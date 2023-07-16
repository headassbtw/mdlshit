#include <mdls.hpp>
#include <JsonTest.h>
#include <conv.hpp>
#include <rapidjson/istreamwrapper.h>
#include <structs.hpp>
#include <binarystream.hpp>
#include <structs.hpp>
#include <argtools.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <rendering/progress.hpp>
#include <vector>
#include <half.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <logger.hpp>
#include <MLUtil.h>

int GetAnimHeaderSize(int flag)
{
	switch (flag)
	{
	case 33: return 18;
	case 32: return 12;
	case 8: return 10;
	case 1: return 10;
	case 12: return 16;
	case 49: return 18;
	case 48: return 12;
	case 24: return 10;
	case 17: return 10;
	case 28: return 16;
	case 0: return 4;

	default: return 0;
	}
}
uint32_t as_uint(const float x) {
	return *(uint32_t*)&x;
}
float as_float(const uint32_t x) {
	return *(float*)&x;
}

float half_to_float(const uint16_t x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const uint32_t e = (x & 0x7C00) >> 10; // exponent
	const uint32_t m = (x & 0x03FF) << 13; // mantissa
	const uint32_t v = as_uint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
	return as_float((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
}

short float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const uint32_t b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
	const uint32_t e = (b & 0x7F800000) >> 23; // exponent
	const uint32_t m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
	return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}

bool IsWhole(double n) {
	if (n == (int)n)
		return true;
	else
		return false;
}

float GetSecondLargest(float n1, float n2, float n3)
{
	float n = 3, num[]{ n1,n2,n3 }, largest, second;

	/* Here we are comparing first two elements of the
	 * array, and storing the largest one in the variable
	 * "largest" and the other one to "second" variable.
	 */
	if (num[0] < num[1]) {
		largest = num[1];
		second = num[0];
	}
	else {
		largest = num[0];
		second = num[1];
	}
	for (int i = 2; i < n; i++) {
		/* If the current array element is greater than largest
		 * then the largest is copied to "second" and the element
		 * is copied to the "largest" variable.
		 */
		if (num[i] > largest) {
			second = largest;
			largest = num[i];
		}
		/* If current array element is less than largest but greater
		 * then second largest ("second" variable) then copy the
		 * element to "second"
		 */
		else if (num[i] > second && num[i] != largest) {
			second = num[i];
		}
	}
	return second;
	return 0;
}

float GetLargestNumber(float n1, float n2, float n3)
{
	//if (n1 > n2 && n1 == n2 || n1 > n2 && n1 > n3 && n2 == n3 || n1 > n2 && n1 < n3 || n1 > n2 && n3 < n2)
	//	return n1;
	//
	//if (n2 > n1 && n2 == n3 || n2 > n1 && n2 > n3 && n1 == n3 || n2 > n1 && n2 < n3 || n2 > n1 && n3 < n1)
	//	return n2;
	//
	//if (n3 > n1 && n3 == n2 || n3 > n1 && n3 > n2 && n1 == n2 || n3 > n1 && n3 < n2 || n3 > n2 && n3 < n1)
	//	return n3;

	if (n1 >= n2 && n1 >= n3)
		return n1;

	if (n2 >= n1 && n2 >= n3)
		return n2;

	if (n3 >= n1 && n3 >= n2)
		return n3;

	return n1;
}

bool contains(std::vector<int> arry, int trgt)
{
	for (int i = 0; i < arry.size(); i++)
	{
		if (trgt == arry[i]) return true;
	}
	return false;
}

template <typename T> int GetTrgtPosInArry(std::vector<T> arry, T trgt)
{
	for (int i = 0; i < arry.size(); i++)
	{
		if (trgt == arry[i]) return i;
	}
	return -1;
}

template <typename T> std::vector<T> ArrayToVector(T* arry, int size)
{
	std::vector<T> _vec;
	for (int i = 0; i < size; i++)
	{
		_vec.push_back(arry[i]);
	}
	return _vec;
}

int FaceType(mstudioruivertmap_t vertMap)
{

	for (int i = 0; i < 3; i++)
	{
		if (vertMap.vertendid == vertMap.vertstartid + 2) return 0;
		if (vertMap.vertextraid == vertMap.vertstartid + 2) return 1;
		if (vertMap.vertendid == vertMap.vertstartid + 2) return 0;
	}


	//Logger::Info("vert1: %d vert2: %d\n", vertMap.vertstartid, vertMap.vertendid);

	//if (vertMap.vertendid == vertMap.vertstartid + (short)1 && vertMap.vertextraid == vertMap.vertendid + (short)1) return 2; //Is 1 2 3 pattern
	//
	//if (vertMap.vertendid == vertMap.vertstartid + (short)2 && vertMap.vertextraid == vertMap.vertstartid + (short)1 || vertMap.vertendid == vertMap.vertstartid - (short)2 && vertMap.vertextraid == vertMap.vertstartid - (short)1) return 2; //Is 1 3 2 pattern
	//
	//if (vertMap.vertendid == vertMap.vertstartid + (short)1 && vertMap.vertextraid == vertMap.vertendid + (short)1) return 2; //Is 1 2 3 pattern
	//
	//if (vertMap.vertendid != vertMap.vertstartid + (short)2 && vertMap.vertendid != vertMap.vertstartid + (short)1) return 1; //Is 1 3 2 pattern
	//
	//if (vertMap.vertendid == vertMap.vertstartid + (short)2 && vertMap.vertextraid == vertMap.vertstartid + (short)1) return 1; //Is 1 3 2 pattern
	//
	//if (vertMap.vertendid = vertMap.vertstartid + (short)2 && vertMap.vertextraid != vertMap.vertstartid + (short)1)
	//{
	//	return 0; //Is quad
	//}


}

std::vector<short> ReadVertMap(mstudioruivertmap_t vertMap)
{
	//Logger::Info("RuiMesh Read: %d  %d\n", vertMap.vertstartid, vertMap.vertendid);
	int tpye = FaceType(vertMap);
	std::vector<short> vertOrder;

	short v1 = vertMap.vertstartid;
	short v2 = vertMap.vertendid;
	short v3 = vertMap.vertextraid;


	//for (int i = 0; i < 3; i++)
	//{
	//	//if(v2 == v1 + 2 || v2 == v1 - 2) Logger::Info("Is 1 3\n");
	//	//if(v1 == v2 + 2 || v1 == v2 - 2) Logger::Info("Is 3 1\n");
	//}


	//bool _132Pos = v2 == v1 + (short)2 && v3 == v1 + (short)1;
	//bool _123Pos = v2 == v1 + (short)1 && v3 == v2 + (short)1;
	//bool _132Neg = v2 == v1 - (short)2 && v3 == v1 - (short)1;
	//bool _123Neg = v2 == v1 - (short)1 && v3 == v2 - (short)1;
	//bool _134Pos = v2 == v1 + (short)2 && v3 != v1 + (short)1;
	//bool _134Neg = v2 == v1 - (short)2 && v3 != v1 - (short)1;
	////bool _143Pos = v3 == v1 + (short)2 && v2 != v1 + (short)1;
	////bool _143Neg = v3 == v1 - (short)2 && v2 != v1 - (short)1;
	//bool _413Pos = v3 == v2 + (short)2 && v1 != v2 + (short)1;
	//bool _413Neg = v3 == v2 - (short)2 && v1 != v2 - (short)1;
	//bool _142Pos = v3 == v1 + (short)1 && v2 != v3 + 1;
	//bool _142Neg = v3 == v1 - (short)1 && v2 != v3 - 1;
	//bool _214Pos = v1 == v2 + (short)1 && v3 != v2 + (short)2 && v3 != v1 + 1;
	//bool _214Neg = v1 == v2 - (short)1 && v3 != v2 - (short)2 && v3 != v1 - 1;
	//bool _124Pos = v2 == v1 - (short)1 && v3 == v1 + 1;
	//bool _124Neg = v2 == v1 - (short)1 && v3 == v1 - (short)2 && v3 == v2 - 1;
	//bool _default = !_132Pos && !_123Pos && !_132Neg && !_123Neg && !_134Pos && !_134Neg;
	//bool _134Pos = v2 == v1 + 2 && v3 != v1 + 1;
	//bool _134Neg = v2 == v1 - 2 && v3 != v1 - 1;
	//bool _142Pos = v3 == v1 + 1 && v2 != v3 + 1 && v2 != v1 + 1 && v2 != v1 - 1 && v2 < v1;
	//bool _142Neg = v3 == v1 - 1 && v2 != v3 - 1 && v2 != v1 - 1;
	//bool _124Pos = v2 == v1 - 1 && v3 == v1 + 1;
	//bool _124Neg = v2 == v1 + 1 && v3 != v1 - 1;
	//bool _241Pos = v1 == v3 - 1 && v2 > v1 && v3 == v1 + 1;
	//bool _421Pos = v2 == v1 - 2 && v3 == v1 - 1;

	bool _134Pos = v2 == v1 + 2 && v3 != v1 + 1;
	bool _134Neg = v2 == v1 - 2 && v3 != v1 - 1;
	bool _132Pos = v2 == v1 + 2 && v3 == v1 + 1;
	bool _132Neg = v2 == v1 - 2 && v3 == v1 - 1;
	bool _123Pos = v2 == v1 + 1 && v3 == v2 + 1;
	bool _123Neg = v2 == v1 - 1 && v3 == v2 - 1;
	bool _142Pos = v3 == v1 + 1 && v2 != v1 + 2 && v2 != v1 - 2;


	//bool _124Pos = v2 == v1 + 1 && v3 == v1 - 1;
	//bool _124Neg = v2 == v1 - 1 && v3 == v1 + 1;
	//
	//bool _421Pos = v3 == v2 + 1 && v1 != v2 - 1 && !_132Neg;
	//bool _421Neg = v3 == v2 - 1 && v1 != v2 + 1;
	//
	//bool _142Pos = v3 == v1 + 1 && v2 != v1 + 2 && !_124Neg && !_134Pos && !_134Neg && !_124Pos;
	//bool _142Neg = v3 == v1 - 1 && v2 != v1 - 2 && !_134Pos && !_134Neg && !_124Pos && !_124Neg;
	
	if (_132Pos) Logger::Info("//Is Quad 132POS\n");
	if (_132Neg) Logger::Info("//Is Quad 132NEG\n");

	if (_123Pos) Logger::Info("//Is Quad 123POS\n");
	//if (_123Neg) Logger::Info("//Is Quad 123NEG\n");

	if (_142Pos) Logger::Info("//Is Quad 142POS VertOrder: %d %d %d - %d %d %d\n", v1, v3 + 1, v3, v2, v3, v3 + 1);
	if (_134Pos) Logger::Info("//Is Quad 134POS VertOrder: %d %d %d - %d %d %d\n", v1, v2, v1 + 1, v3, v1 + 1, v2);
	if (_134Neg) Logger::Info("//Is Quad 134NEG VertOrder: %d %d %d - %d %d %d\n", v1, v2, v1 - 1, v3, v1 - 1, v2);

	//if (_124Pos) Logger::Info("//Is Quad 124POS\n");
	//if (_124Neg) Logger::Info("//Is Quad 124NEG\n");
	//
	//if (_421Pos) Logger::Info("//Is Quad 421POS\n");
	//if (_421Neg) Logger::Info("//Is Quad 421NEG\n");
	//
	//if (_142Pos) Logger::Info("//Is Quad 142POS\n");
	//if (_142Neg) Logger::Info("//Is Quad 142NEG\n");


	//if (_134Pos)
	//{
	//	Logger::Info("//Is Quad 134POS\n");
	//	vertOrder.push_back(v1);
	//	vertOrder.push_back(v2);
	//	vertOrder.push_back(v1 + 1);
	//	//vertOrder.push_back(vertMap.vertstartid + (short)1);
	//	vertOrder.push_back(v3);
	//	vertOrder.push_back(v1 + 1);
	//	vertOrder.push_back(v2);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_134Neg)
	//{
	//	Logger::Info("//Is Quad 134NEG\n");
	//	vertOrder.push_back(v1);
	//	vertOrder.push_back(v2);
	//	vertOrder.push_back(v1 - 1);
	//	//vertOrder.push_back(vertMap.vertstartid + (short)1);
	//	vertOrder.push_back(v3);
	//	vertOrder.push_back(v1 - 1);
	//	vertOrder.push_back(v2);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_142Pos)
	//{
	//	Logger::Info("//Is Quad 142POS\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertextraid + (short)1); //vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_142Neg)
	//{
	//	Logger::Info("//Is Quad 142NEG\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid - (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_124Pos)
	//{
	//	Logger::Info("//Is Quad 124POS\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid - (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_241Pos)
	//{
	//	Logger::Info("//Is Quad 241POS\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid - (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_421Pos)
	//{
	//	Logger::Info("//Is Quad 421POS\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid - (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_124Neg)
	//{
	//	Logger::Info("//Is Quad 124NEG\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid - (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_214Pos && !_142Neg && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
	//{
	//	//Logger::Info("Is Quad\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertstartid + (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_124Pos)
	//{
	//	//Logger::Info("Is Quad\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid - (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_124Neg && !_142Neg && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
	//{
	//	//Logger::Info("Is Quad\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertendid - (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_214Neg && !_142Pos && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
	//{
	//	//Logger::Info("Is Quad\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertstartid - (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_143Pos && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
	//{
	//	//Logger::Info("Is Quad\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid + (short)1);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertstartid + (short)1);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_143Neg && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
	//{
	//	//Logger::Info("Is Quad\n");
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertstartid - (short)1);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertstartid - (short)1);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_413Pos)
	//{
	//	Logger::Info("//Is Quad 413POS\n");
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertendid + (short)1);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid + (short)1);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}
	//if (_413Neg)
	//{
	//	Logger::Info("//Is Quad 413NEG\n");
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertextraid);
	//	vertOrder.push_back(vertMap.vertendid - (short)1);
	//	vertOrder.push_back(vertMap.vertstartid);
	//	vertOrder.push_back(vertMap.vertendid);
	//	vertOrder.push_back(vertMap.vertendid - (short)1);
	//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
	//	return vertOrder;
	//}

	vertOrder.push_back(vertMap.vertstartid);
	vertOrder.push_back(vertMap.vertendid);
	vertOrder.push_back(vertMap.vertextraid);
	return vertOrder;
}

//std::vector<std::vector<short>> ReadVertMap2(std::vector<mstudioruivertmap_t> vertMaps)
//{
//	//Logger::Info("RuiMesh Read: %d  %d\n", vertMap.vertstartid, vertMap.vertendid);
//	std::vector<short> quadVertsUsed;
//	std::vector<short> vertOrder;
//	std::vector<std::vector<short>> vertOrders;
//
//	for (int i = 0; i < vertMaps.size(); i++)
//	{
//
//	}
//
//	for (int i = 0; i < vertMaps.size(); i++)
//	{
//		short v1 = vertMaps[i].vertstartid;
//		short v2 = vertMaps[i].vertendid;
//		short v3 = vertMaps[i].vertextraid;
//
//		if (Utility::check::contains(quadVertsUsed, v1) || Utility::check::contains(quadVertsUsed, v2) || Utility::check::contains(quadVertsUsed, v3))
//		{
//
//		}
//
//
//		bool _134Pos = v2 == v1 + 2 && v3 != v1 + 1;
//		bool _134Neg = v2 == v1 - 2 && v3 != v1 - 1;
//		bool _132Pos = v2 == v1 + 2 && v3 == v1 + 1;
//		bool _132Neg = v2 == v1 - 2 && v3 == v1 - 1;
//		bool _123Pos = v2 == v1 + 1 && v3 == v2 + 1;
//		bool _123Neg = v2 == v1 - 1 && v3 == v2 - 1;
//		bool _142Pos = v3 == v1 + 1 && v2 != v1 + 2 && v2 != v1 - 2;
//
//		if (_132Pos) Logger::Info("//Is Quad 132POS\n");
//		if (_132Neg) Logger::Info("//Is Quad 132NEG\n");
//
//		if (_123Pos) Logger::Info("//Is Quad 123POS\n");
//		//if (_123Neg) Logger::Info("//Is Quad 123NEG\n");
//
//		if (_142Pos) Logger::Info("//Is Quad 142POS VertOrder: %d %d %d - %d %d %d\n", v1, v3 + 1, v3, v2, v3, v3 + 1);
//		if (_134Pos) Logger::Info("//Is Quad 134POS VertOrder: %d %d %d - %d %d %d\n", v1, v2, v1 + 1, v3, v1 + 1, v2);
//		if (_134Neg) Logger::Info("//Is Quad 134NEG VertOrder: %d %d %d - %d %d %d\n", v1, v2, v1 - 1, v3, v1 - 1, v2);
//
//		vertOrder.push_back(vertMaps[i].vertstartid);
//		vertOrder.push_back(vertMaps[i].vertendid);
//		vertOrder.push_back(vertMaps[i].vertextraid);
//		vertOrders.push_back(vertOrder);
//		vertOrder.clear();
//
//	}
//	return vertOrders;
//}

#pragma region MDLV53

void RUIMeshToSmd(mstudioruimesh_t ruiMesh)
{
	int faces = ruiMesh.numfaces;
	int vertNum = 0;
	Logger::Info("version 1\n");
	Logger::Info("nodes\n");
	for (int i = 0; i < ruiMesh.numvertices; i++)
	{
		Logger::Info("%d \"Vert%d\" %d\n", i, i, i - 1);
	}
	Logger::Info("end\n");
	Logger::Info("skeleton\n");
	Logger::Info("time 0\n");
	for (int i = 0; i < ruiMesh.numvertices; i++)
	{
		Logger::Info("%d  0.000000 0.000000 0.000000  0.000000 -0.000000 0.000000\n", i);
	}
	Logger::Info("end\n");
	Logger::Info("triangles\n");
	Logger::Info("FACE%d \n", 0);
	//Logger::Info("RuiMesh Read: %d  %d\n", ruiMesh.vertexmap[0].vertstartid, ruiMesh.vertexmap[0].vertendid);
	int faceNum = 1;
	for (int i = 0; i < ruiMesh.numvertices; i++)
	{
		//Logger::Info("FACE%d \n", faceNum);
		if (vertNum >= 3)
		{
			Logger::Info("FACE%d \n", faceNum);
			faceNum++;
			vertNum = 0;
		}
		mstudioruivert_t vert1 = ruiMesh.vertex[i];
		Logger::Info("%d %f %f %f 0.000000 0.000000 0.000000	0.5 1	1 %d 1 //vert:	%d\n", i, vert1.vertexpos.x, vert1.vertexpos.y, vert1.vertexpos.z, i, i);
		vertNum++;
		//for (int j = 0; j < ruiMesh.numvertices; j++)
		//{
		//	mstudioruivert_t vert1 = ruiMesh.vertex[vertNum];
		//	Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert1.vertexpos.x, vert1.vertexpos.y, vert1.vertexpos.z);
		//	vertNum++;
		//}
		//faceNum++;
	}
	if (!IsWhole(ruiMesh.numvertices / static_cast<double>(3)))
	{
		int idk = 0;
		double sizeCheck2 = 3 * ((int)(ruiMesh.numvertices / static_cast<double>(3)) + 1);
		idk = sizeCheck2 - (int)ruiMesh.numvertices;
		for (int i = 0; i < idk; i++)
		{
			Logger::Info("%d %f %f %f 0.000000 0.000000 0.000000	0.5 1	1 %d 1 //vert:	%d\n", 0, 0, 0, 0, 0, 0);
		}
	}


	//for (int i = 0; i < ruiMesh.numfaces; i++)
	//{
	//	mstudioruivertmap_t map = ruiMesh.vertexmap[i];
	//	//Logger::Info("RuiMesh Read: %d  %d\n", ruiMesh.vertexmap[i].vertstartid, ruiMesh.vertexmap[i].vertendid);
	//	std::vector<short> vertMap = ReadVertMap(map);
	//	Logger::Info("FACE%d \n", i);
	//	if (vertMap.size() > 3)
	//	{
	//		int v1 = vertMap[0];
	//		int v2 = vertMap[1];
	//		int v3 = vertMap[2];
	//		int v4 = vertMap[3];
	//		int v5 = vertMap[4];
	//		int v6 = vertMap[5];
	//		//Logger::Info("Vert4 Read: %d \n", vertMap[3]);
	//		//Logger::Info("Vert4 Read: %d \n", v4);
	//
	//		mstudioruivert_t vert1 = ruiMesh.vertex[v1];
	//		mstudioruivert_t vert2 = ruiMesh.vertex[v2];
	//		mstudioruivert_t vert3 = ruiMesh.vertex[v3];
	//
	//		mstudioruivert_t vert4 = ruiMesh.vertex[v4];
	//		mstudioruivert_t vert5 = ruiMesh.vertex[v5];
	//		mstudioruivert_t vert6 = ruiMesh.vertex[v6];
	//
	//		Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.5 1 1 0 1\n", vert1.vertexpos.x, vert1.vertexpos.y, vert1.vertexpos.z);
	//		Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.5 1 1 0 1\n", vert2.vertexpos.x, vert2.vertexpos.y, vert2.vertexpos.z);
	//		Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.5 1 1 0 1\n", vert3.vertexpos.x, vert3.vertexpos.y, vert3.vertexpos.z);
	//		Logger::Info("FACE%d Quad \n", i);
	//		Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.5 1 1 0 1\n", vert4.vertexpos.x, vert4.vertexpos.y, vert4.vertexpos.z);
	//		Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.5 1 1 0 1\n", vert5.vertexpos.x, vert5.vertexpos.y, vert5.vertexpos.z);
	//		Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.5 1 1 0 1\n", vert6.vertexpos.x, vert6.vertexpos.y, vert6.vertexpos.z);
	//	}
	//	else
	//	{
	//		int v1 = vertMap[0];
	//		int v2 = vertMap[1];
	//		int v3 = vertMap[2];
	//
	//		mstudioruivert_t vert1 = ruiMesh.vertex[v1];
	//		mstudioruivert_t vert2 = ruiMesh.vertex[v2];
	//		mstudioruivert_t vert3 = ruiMesh.vertex[v3];
	//
	//		Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.5 1 1 0 1 //vert: %d\n", vert1.vertexpos.x, vert1.vertexpos.y, vert1.vertexpos.z, v1);
	//		Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.5 1 1 0 1 //vert: %d\n", vert2.vertexpos.x, vert2.vertexpos.y, vert2.vertexpos.z, v2);
	//		Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.5 1 1 0 1 //vert: %d\n", vert3.vertexpos.x, vert3.vertexpos.y, vert3.vertexpos.z, v3);
	//	}
	//}
	Logger::Info("end\n");


}

std::string ReadMDLString(BinaryReader* Stream, int pos)
{
	int startPos = Stream->Position();
//	int size = 0;

	int defSize = 100;

	std::vector<char> vec;
	Stream->seek(pos);
	if (defSize > Stream->size - pos) defSize = Stream->size - pos; //Prevents overflow - Liberty.

	for (int i = 0; i < defSize; i++)
	{
		char value; Stream->read(&value, 1);
		if (&value == ".")
		{
			break;
		}
		else
		{
			vec.push_back(value);
//			size++;
		}
	}
	std::string string(vec.begin(), vec.end());
	Stream->seek(startPos);
	return string;

}

template <typename T> int GetJiggleBoneCount(T mdl, bool debug)
{
	const char* type = typeid(mdl).name(); if (debug) Logger::Info("Type: %s\n", (std::string)type);
	int jiggleBoneCount = 0;

	if (type == typeid(MDL::v53Mdl).name() || type == typeid(MDL::v49Mdl).name() || type == typeid(MDL::v49Mdl).name())
	{
		int version = mdl.mdlhdr.version;
		switch (version)
		{
		case 53: for (int i = 0; i < mdl.mdlhdr.numbones; i++) { if (mdl.bones[i].proctype == 5) jiggleBoneCount++; } if (debug) Logger::Info("JiggleBone Count: %d\n", jiggleBoneCount); return jiggleBoneCount;

		case 52: for (int i = 0; i < mdl.mdlhdr.numbones; i++) { if (mdl.bones[i].proctype == 5) jiggleBoneCount++; } if (debug) Logger::Info("JiggleBone Count: %d\n", jiggleBoneCount); return jiggleBoneCount;

		case 49: for (int i = 0; i < mdl.mdlhdr.numbones; i++) { if (mdl.bones[i].proctype == 5) jiggleBoneCount++; } if (debug) Logger::Info("JiggleBone Count: %d\n", jiggleBoneCount); return jiggleBoneCount;

		default: return jiggleBoneCount;
		}
	}
	Logger::Error("No compatible mdl class was passed. Returning 0. Wrong Type: \n", type);
	return 0;
}

template <typename T> int GetHitboxCount(T mdl, bool debug)
{
	const char* type = typeid(mdl).name();
	int hitboxCount = 0;

	if (type == typeid(MDL::v53Mdl).name() || type == typeid(MDL::v49Mdl).name() || type == typeid(MDL::v49Mdl).name())
	{
		int version = mdl.mdlhdr.version;
		switch (version)
		{
		case 53: for (int i = 0; i < mdl.mdlhdr.numhitboxsets; i++) { if (mdl.hitboxsets[i].numhitboxes > 0) hitboxCount += mdl.hitboxsets[i].numhitboxes; } if (debug) Logger::Info("Hitbox Count: %d\n", hitboxCount); return hitboxCount;

		case 52: for (int i = 0; i < mdl.mdlhdr.numhitboxsets; i++) { if (mdl.hitboxsets[i].numhitboxes > 0) hitboxCount += mdl.hitboxsets[i].numhitboxes; } if (debug) Logger::Info("Hitbox Count: %d\n", hitboxCount); return hitboxCount;

		case 49: for (int i = 0; i < mdl.mdlhdr.numhitboxsets; i++) { if (mdl.hitboxsets[i].numhitboxes > 0) hitboxCount += mdl.hitboxsets[i].numhitboxes; } if (debug) Logger::Info("Hitbox Count: %d\n", hitboxCount); return hitboxCount;

		default: return hitboxCount;
		}
	}
	Logger::Error("No compatible mdl class was passed. Returning 0. Wrong Type: \n", type);
	return 0;
}

template <typename T> int GetModelCount(T mdl, bool debug)
{
	const char* type = typeid(mdl).name(); if (debug) Logger::Info("Type: %s\n", type);
	int modelCount = 0;

	if (type == typeid(MDL::v53Mdl).name() || type == typeid(MDL::v49Mdl).name() || type == typeid(MDL::v49Mdl).name())
	{
		int version = mdl.mdlhdr.version;
		switch (version)
		{
		case 53: for (int i = 0; i < mdl.mdlhdr.numbodyparts; i++) { if (mdl.bodyparts[i].nummodels > 0) modelCount += mdl.bodyparts[i].nummodels; } if (debug) Logger::Info("Model Count: %d\n", modelCount); return modelCount;

		case 52: for (int i = 0; i < mdl.mdlhdr.numbodyparts; i++) { if (mdl.bodyparts[i].nummodels > 0) modelCount += mdl.bodyparts[i].nummodels; } if (debug) Logger::Info("Model Count: %d\n", modelCount); return modelCount;

		case 49: for (int i = 0; i < mdl.mdlhdr.numbodyparts; i++) { if (mdl.bodyparts[i].nummodels > 0) modelCount += mdl.bodyparts[i].nummodels; } if (debug) Logger::Info("Model Count: %d\n", modelCount); return modelCount;

		default: return modelCount;
		}
	}
	Logger::Error("No compatible mdl class was passed. Returning 0. Wrong Type: \n", type);
	return 0;
}

void AddDistToAnimData(mstudioanimdata_t_v49 vec, int extra)
{
	for (int i = 0; i < extra; i++)
	{
		vec.arry.push_back((std::byte)0);
	}
}


MDL::v53Mdl::v53Mdl()
{
	Logger::Info("test\n");
}

MDL::v53Mdl::v53Mdl(studiohdr_t_v53 _mdlhdr, mayabakery_t _mayabakery, std::vector<mstudiobone_t_v53> _bones, std::vector<mstudiojigglebone_t_v49> _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, std::vector<mstudioattachment_t_v49> _attachments, std::vector<mstudiohitboxset_t_v53> _hitboxsets, std::vector<mstudiobbox_t_v53> _hitboxes, mstudiobonenametable_t_v49 _bonenametable, std::vector<mstudioanimdesc_t_v53> _animdescs, std::vector<mstudioanim_t_v53> _anims, std::vector<sectionindexes_t_v53> _sectionindexes, std::vector<mstudioanim_t_v53> _sections, std::vector<mstudioikrule_t_v53> _ikrules, std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors, std::vector<mstudiocompressedikerror_t_v52> _v52compressedikerrors, std::vector<mstudioikerror_t_v49> _ikerrors, std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe, std::vector<mstudioseqdesc_t_v53> _seqdescs, std::vector<blendgroup_t_v49> _blends, std::vector<posekey_t_v49> _posekeys, std::vector<mstudioevent_t_v49> _events, std::vector<mstudioautolayer_t_v49> _autolayers, std::vector<mstudioactivitymodifier_t_v53> _activitymodifiers, std::vector<seqweightlist_t_v49> _seqweightlist, std::vector<mstudionodename_t_v49> _nodenames, std::vector<mstudionodedata_v49> _nodes, std::vector<mstudiobodyparts_t_v49> _bodyparts, std::vector<mstudiomodel_t_v49>	_models, std::vector<mstudiomesh_t_v49> _meshes, std::vector<mstudioikchain_t_v53> _ikchains, std::vector<mstudioiklink_t_v49> _iklinks, std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs, std::vector<mstudiomodelgroup_t_v49> _includedmodels, std::vector<mstudiorruiheader_t> _ruihdrs, std::vector<mstudioruimesh_t> _ruimeshes, std::vector<mstudiotexture_t_v53> _textures, std::vector<mstudiotexturedir_t_v49> _cdtextures, std::vector<mstudioskingroup_t_v49> _skingroups, mstudiokeyvalues_t_v49 _keyvalues, std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v53 _linearbonedata, mstudiostringtable_t_v52 _stringtable, pertriheader_t _pertriheader, std::vector<aabbnode> _aabbnodes, std::vector<aabbleaf> _aabbleaves, aabbcollisionmask _aabbverts)
{
	mdlhdr = _mdlhdr;
	mayabakery = _mayabakery;
	bones = _bones;
	jigglebones = _jigglebones;
	boneflexdrivers = _boneflexdrivers;
	attachments = _attachments;
	hitboxsets = _hitboxsets;
	hitboxes = _hitboxes;
	bonenametable = _bonenametable;
	animdescs = _animdescs;
	anims = _anims;
	sectionindexes = _sectionindexes;
	sections = _sections;
	ikrules = _ikrules;
	compressedikerrors = _compressedikerrors;
	v52compressedikerrors = _v52compressedikerrors;
	ikerrors = _ikerrors;
	ikrulezeroframe = _ikrulezeroframe;
	seqdescs = _seqdescs;
	blends = _blends;
	posekeys = _posekeys;
	events = _events;
	autolayers = _autolayers;
	activitymodifiers = _activitymodifiers;
	seqweightlist = _seqweightlist;
	nodenames = _nodenames;
	nodes = _nodes;
	bodyparts = _bodyparts;
	models = _models;
	meshes = _meshes;
	ikchains = _ikchains;
	iklinks = _iklinks;
	poseparamdescs = _poseparamdescs;
	includedmodels = _includedmodels;
	ruihdrs = _ruihdrs;
	ruimeshes = _ruimeshes;
	cdtextures = _cdtextures;
	textures = _textures;
	skingroups = _skingroups;
	keyvalues = _keyvalues;
	srcbonetransforms = _srcbonetransforms;
	linearbone = _linearbone;
	linearbonedata = _linearbonedata;
	stringtable = _stringtable;
	pertriheader = _pertriheader;
	aabbnodes = _aabbnodes;
	aabbleaves = _aabbleaves;
	aabbverts = _aabbverts;
}

void MDL::v53Mdl::v53ExtractRUIMesh(char* filename) //There is a much much better way of doing this by just writing the file as it reads the structs and using the indexes to calculate padding. -Liberty
{
	BinaryReader* Stream = new BinaryReader(filename);
	
	Stream->seek(0);
	std::vector<mstudiorruiheader_t>					ruiHdrs;
	if (this->mdlhdr.numruimeshes > 0)
	{
		Stream->seek(0);
		Stream->seek(this->mdlhdr.ruimeshindex);
		for (int i = 0; i < ruimeshes.size(); i++)
		{
			mstudiorruiheader_t ruiHdr; Stream->Read(&ruiHdr);
			ruiHdrs.push_back(ruiHdr);
			Logger::Info("RuiHdr Read: %d\n", i);
		}

		Stream->seek(this->mdlhdr.ruimeshindex + ruiHdrs[0].ruimeshindex);
		for (int j = 0; j < ruiHdrs.size(); j++)
		{
			int ruiPos = Stream->Position();
			mstudioruimesh_t ruiMesh; Stream->Read(&ruiMesh);

			std::string szName = Stream->ReadNullTermStrTrgt(ruiPos + 32, false);

			Logger::Info("RuiMesh Read: %s\n", szName.c_str());
			Logger::Info("RuiMesh Read: %d\n", j);
		}
		std::string fileName = Stream->_filename;
		fileName.resize(fileName.size() - 4);
		fileName.insert(fileName.size(), ".rui");
		int ruiEnd = Stream->Position();
		Stream->seek(this->mdlhdr.ruimeshindex);
		BinaryWriter Bing = BinaryWriter(fileName.c_str());
		char* data = new char[ruiEnd - this->mdlhdr.ruimeshindex];
		Stream->read(data, ruiEnd - this->mdlhdr.ruimeshindex);
		Bing.write(data, ruiEnd - this->mdlhdr.ruimeshindex);
		
	}
	else{
		Logger::Debug("No RUI Meshes!\n");
	}
	delete Stream;
	Logger::Info("Finished dumping RUI Mesh\n");
}

void MDL::v53Mdl::v53ExtractSrcBoneTransforms(BinaryReader* Stream)
{
	if (mdlhdr.numsrcbonetransform > 0)
	{
		std::string fileName = Stream->_filename;
		fileName.resize(fileName.size() - 4);
		fileName.insert(fileName.size(), ".srcbtf");
		BinaryWriter Bing = BinaryWriter(fileName.c_str());

		for (int i = 0; i < mdlhdr.numsrcbonetransform; i++)
		{
			Bing.Write(srcbonetransforms[i]);
		}
	}
}

void MDL::v53Mdl::v53ExtractAABBTree(BinaryReader* Stream)
{
	if (mdlhdr.aabbindex > 0)
	{
		std::string fileName = Stream->_filename;
		fileName.resize(fileName.size() - 4);
		fileName.insert(fileName.size(), ".aabb");
		Stream->seek(mdlhdr.aabbindex);
		BinaryWriter Bing = BinaryWriter(fileName.c_str());
		Stream->Read(&pertriheader);
		Bing.Write(pertriheader);

		if (mdlhdr.numaabbnodes > 0)
		{
			for (int i = 0; i < mdlhdr.numaabbnodes; i++)
			{
				Bing.Write(aabbnodes[i]);
			}
			//		Logger::Info("Pos Read: %d\n", Stream->Position());
		}

		if (mdlhdr.numaabbleaves > 0)
		{
			for (int i = 0; i < mdlhdr.numaabbleaves; i++)
			{
				Bing.Write(aabbleaves[i]);
			}
		}

		if (mdlhdr.numaabbverts > 0)
		{
			Bing.Write(aabbverts);
		}

	}
}

MDL::v53Mdl MDL::v53Mdl::_v53Mdl(BinaryReader* Stream, bool debug)
{
	Stream->seek(0);
	#pragma region HDR
	studiohdr_t_v53										mdlhdr; Stream->Read(&mdlhdr);
	#pragma endregion
	#pragma region VARS
	mayabakery_t										mayabakery;
	std::vector<mstudiobone_t_v53>						bones;
	std::vector<mstudiojigglebone_t_v49>				jigglebones;
	std::vector<mstudioboneflexdriver_t_v49>			boneflexdrivers;
	std::vector<mstudioattachment_t_v49>				attachments;
	std::vector<mstudiohitboxset_t_v53>					hitboxsets;
	std::vector<mstudiobbox_t_v53>						hitboxes;
	mstudiobonenametable_t_v49							bonenametable;
	std::vector<mstudioanimdesc_t_v53>					animdescs;
	std::vector<mstudioanim_t_v53>						anims;
	std::vector<sectionindexes_t_v53>					sectionindexes;
	std::vector<mstudioanim_t_v53>						sections;
	std::vector<mstudioikrule_t_v53>					ikrules;
	std::vector<mstudiocompressedikerror_t_v49>			compressedikerrors;
	std::vector<mstudiocompressedikerror_t_v52>			v52compressedikerrors;
	std::vector<mstudioikerror_t_v49>					ikerrors;
	std::vector<mstudioikrulezeroframe_t_v49>			ikrulezeroframe;
	std::vector<mstudioseqdesc_t_v53>					seqdescs;
	std::vector<blendgroup_t_v49>						blends;
	std::vector<posekey_t_v49>							posekeys;
	std::vector<mstudioevent_t_v49>						events;
	std::vector<mstudioautolayer_t_v49>					autolayers;
	std::vector<mstudioactivitymodifier_t_v53>			activitymodifiers;
	std::vector<seqweightlist_t_v49>					seqweightlist;
	std::vector<mstudionodename_t_v49>					nodenames;
	std::vector<mstudionodedata_v49>					nodes;
	std::vector<mstudiobodyparts_t_v49>					bodyparts;
	std::vector<mstudiomodel_t_v49>						models;
	std::vector<mstudiomesh_t_v49>						meshes;
	std::vector<mstudioikchain_t_v53>					ikchains;
	std::vector<mstudioiklink_t_v49>					iklinks;
	std::vector<mstudioposeparamdesc_t_v49>				poseparamdescs;
	std::vector<mstudiomodelgroup_t_v49>				includedmodels;
	std::vector<mstudiorruiheader_t>					ruiHdrs;
	std::vector<mstudioruimesh_t>						ruiMeshes;
	std::vector<mstudiotexture_t_v53>					textures;
	std::vector<mstudiotexturedir_t_v49>				cdtextures;
	std::vector<mstudioskingroup_t_v49>					skingroups;
	mstudiokeyvalues_t_v49								keyvalues;
	std::vector<mstudiosrcbonetransform_t_v49>			srcbonetransforms;
	mstudiolinearbone_t_v49								linearbone {};
	mstudiolinearbonedata_t_v53							linearbonedata{};
	pertriheader_t										pertriheader;
	std::vector<aabbnode>								aabbnodes;
	std::vector<aabbleaf>								aabbleaves;
	aabbcollisionmask									aabbverts;

	int numjigglebones = 0;

	int numhitboxes = 0;

	int nummodels = 0;

	int nummeshes = 0;

	int numiklinks = 0;

	int numiklocks = 0;

	int numikrules = 0;

	int numTextures = mdlhdr.numtextures;

	int numikbones = 0;

	#pragma endregion
	#pragma region MAYABAKERY
	if (mdlhdr.boneindex > mdlhdr.mayaindex && mdlhdr.mayaindex != 0)
	{
		Stream->seek(mdlhdr.mayaindex);
		int mayaBakerySize = mdlhdr.boneindex - mdlhdr.mayaindex;
		Stream->Read(&mayabakery, mayaBakerySize);
		Logger::Info("MayaBakery Read: \n");
	}
	#pragma endregion
	#pragma region BONES
	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlhdr.boneindex);
		for (int i = 0; i < mdlhdr.numbones; i++)
		{
			int bonePos = mdlhdr.boneindex + 244 * i;
			Stream->seek(bonePos);

			mstudiobone_t_v53 bone; Stream->Read(&bone);
			if (bone.proctype == 5) numjigglebones++;
			if (bone.flags & BONE_USED_BY_IKCHAIN)
			{
				numikbones++;
				Logger::Info("IKBone Read: %d)", i);
			}
			std::string szName = Stream->ReadNullTermStrTrgt(bonePos + bone.sznameindex, false);
			bone.szname = szName;
			bones.push_back(bone);

			Logger::Info("Bone Read: %s | %d\n", szName.c_str(), i);
		}
		Logger::Info("IkBones: %d\n", numikbones);
	}
	#pragma endregion
	#pragma region JIGGLEBONES
	if (numjigglebones > 0)
	{
		int jiggleBonePos = mdlhdr.boneindex + 244 * mdlhdr.numbones;

		Stream->seek(jiggleBonePos);

		for (int i = 0; i < numjigglebones; i++)
		{
			mstudiojigglebone_t_v49 jigglebone; Stream->Read(&jigglebone);
			jigglebones.push_back(jigglebone);
			Logger::Info("JiggleBone Read: %d\n", i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
	#pragma endregion
	#pragma region ATTACHMENTS
	if (mdlhdr.numlocalattachments > 0)
	{
		Stream->seek(mdlhdr.localattachmentindex);
		for (int i = 0; i < mdlhdr.numlocalattachments; i++)
		{
			int attachmentPos = mdlhdr.localattachmentindex + 92 * i;
			Stream->seek(attachmentPos);

			mstudioattachment_t_v49 attachment; Stream->Read(&attachment);

			std::string szName = Stream->ReadNullTermStrTrgt(attachmentPos + attachment.sznameindex, false);

			attachment.szname = szName;

			attachments.push_back(attachment);

			Logger::Info("Attachment Read: %s\n", szName.c_str());
			Logger::Info("Attachment Read: %d\n", i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
	#pragma endregion
	#pragma region HITBOXSETS
	if (mdlhdr.numhitboxsets > 0)
	{
		Stream->seek(mdlhdr.hitboxsetindex);
		if (mdlhdr.numhitboxsets > 0)
		{
			Stream->seek(mdlhdr.hitboxsetindex);
			for (int i = 0; i < mdlhdr.numhitboxsets; i++)
			{
				int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * i;
				Stream->seek(hitboxsetPos);

				mstudiohitboxset_t_v53 hitboxset; Stream->Read(&hitboxset);

				if (hitboxset.numhitboxes > 0) numhitboxes += hitboxset.numhitboxes;

				Stream->seek(hitboxsetPos + hitboxset.hitboxindex);
				for (int j = 0; j < hitboxset.numhitboxes; j++)
				{
					int hitboxPos = hitboxsetPos + hitboxset.hitboxindex + 68 * i;
					Stream->seek(hitboxPos);
					mstudiobbox_t_v53 hitbox; Stream->Read(&hitbox);

					std::string szHitboxName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.szhitboxnameindex, false);
					std::string szKvName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.keyvalueindex, false);
					hitbox.szname = szHitboxName;
					hitbox.szkvname = szKvName;

					hitboxset.hitboxes.push_back(hitbox);
				}

				std::string szName = Stream->ReadNullTermStrTrgt(hitboxsetPos + hitboxset.sznameindex, false);
				hitboxset.szname = szName;

				hitboxsets.push_back(hitboxset);
				Logger::Info("HitboxSet Read: %s : %d\n", szName.c_str(), i);
			}
		}
	#pragma endregion
	#pragma region HITBOXES
		if (numhitboxes > 0)
		{
			for (int i = 0; i < numhitboxes; i++)
			{
				int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * mdlhdr.numhitboxsets;
				int hitboxPos = hitboxsetPos + 68 * i;
				Stream->seek(hitboxPos);
				mstudiobbox_t_v53 hitbox; Stream->Read(&hitbox);

				std::string szName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.szhitboxnameindex, false);
				std::string szKvName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.keyvalueindex, false);

				hitbox.szname = szName;
				hitbox.szkvname = szKvName;

				Logger::Info("Hitbox Read: %s\n", szName.c_str());
				Logger::Info("Hitbox Read: %s\n", szKvName.c_str());
				hitboxes.push_back(hitbox);
				Logger::Info("Hitbox Read: %d\n", i);
			}
		}
		//Logger::Info("strPos: %d\n", Stream->Position());

	}
	#pragma endregion
	#pragma region BONENAMETABLE
	if (mdlhdr.numbones > 0)
	{
		Stream->Read(&bonenametable, mdlhdr.numbones);
		Logger::Info("BoneTable Read: %d\n", mdlhdr.numbones);
	}
	#pragma endregion
	#pragma region ANIMDESCS
	if (mdlhdr.numlocalanim > 0)
	{
		Stream->seek(mdlhdr.localanimindex);
		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int animDescPos = mdlhdr.localanimindex + 92 * i;
			Stream->seek(animDescPos);

			mstudioanimdesc_t_v53 animdesc; Stream->Read(&animdesc);

			std::string szName = Stream->ReadNullTermStrTrgt(animDescPos + animdesc.sznameindex, false);

			animdesc.szname = szName;

			animdescs.push_back(animdesc);

			Logger::Info("AnimDesc Read: %s\n", szName.c_str());
			Logger::Info("AnimDesc Read: %d\n", i);
			//Logger::Info("strPos: %d\n", Stream->Position());
		}
	#pragma endregion
	#pragma region ANIMS

		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int boneHeaderNum = 0;
			int animDescPos = mdlhdr.localanimindex + 92 * i;
			int animPos = animDescPos + animdescs[i].animindex;

			if (animdescs[i].sectionindex == 0)
			{

				Stream->seek(animPos);
				//Logger::Info("strPos: %d\n", Stream->Position());
				for (int j = 0; j < mdlhdr.numlocalanim * mdlhdr.numbones; j++)
				{
					int dist = 0;
					int nextAnim = i + 1;
					int hdrStartPos = Stream->Position();
					//Logger::Info("hdrStartPos: %d\n", hdrStartPos);
					mstudioanim_t_v53 anim; Stream->Read(&anim);
					anims.push_back(anim);
					mstudioanimdata_t_v49 animData;

					//Logger::Info("HdrNum: %d\n", j);
					//Logger::Info("Bone: %d\n", anim.bone);
					//Logger::Info("Flags Read: %d\n", anim.flags);
					//Logger::Info("Unk Read: %d\n", anim.unk);
					//Logger::Info("RawRot Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot.x, anim.rawrot.y, anim.rawrot.z, anim.rawrot.wneg);
					//Logger::Info("RawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
					//Logger::Info("RawScale Read: X: %d Y: %d Z: %d\n", anim.rawscale.x, anim.rawscale.y, anim.rawscale.z);
					//Logger::Info("RotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
					//Logger::Info("PosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);
					//Logger::Info("ScalePtr Read: X: %d Y: %d Z: %d\n", anim.animscale.offset.x, anim.animscale.offset.y, anim.animscale.offset.z);
					//Logger::Info("NextOffset Read: %d\n", anim.nextoffset);

					int headerSize = 32;
					int hdrEndPos = Stream->Position();
					//Logger::Info("hdrEndPos: %d\n", hdrEndPos);
					boneHeaderNum++;
					if (anim.nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - hdrEndPos;
						if (nextAnim > mdlhdr.numlocalanim) dist = mdlhdr.localseqindex - hdrEndPos;

						//Logger::Info("SeqIndx: %d\n", mdlhdr.localseqindex);
						//Logger::Info("AnimDataSizeEnd: %d\n", dist);
						Stream->Read(&animData, dist);
						Logger::Info("Anim Read: %d\n", i);
						break;
					}
					dist = anim.nextoffset - headerSize;
					//Logger::Info("AnimDataSize: %d\n", dist);
					Stream->Read(&animData, dist);

				}

			}
	#pragma endregion
	#pragma region SECTIONS
			if (animdescs[i].sectionindex > 0)
			{
				int secBoneHeaderNum = 0;
				int animDescPos = mdlhdr.localanimindex + 92 * i;
				int animPos = animDescPos + animdescs[i].animindex;
				int secPos = animDescPos + animdescs[i].sectionindex;
				int frames = animdescs[i].numframes;
				int secFrames = animdescs[i].sectionframes;
				std::vector<sectionindexes_t_v53> idxs;

				int secNumber = (frames / secFrames) + 2;

				Stream->seek(secPos);

				for (int j = 0; j < secNumber; j++)
				{

					sectionindexes_t_v53 sectionIdx; Stream->Read(&sectionIdx);
					//Logger::Info("secIdx: %d\n", sectionIdx.sectionoffset);
					idxs.push_back(sectionIdx);
					sectionindexes.push_back(sectionIdx);
				}

				for (int j = 0; j < secNumber; j++)
				{
					sectionindexes_t_v53 sectionIdx = idxs[j];
					int nextSection = j + 1;
					//Logger::Info("secIdxPos: %d\n", animDescPos + sectionIdx.sectionoffset);
					Stream->seek(animDescPos + sectionIdx.sectionoffsets);


					for (int k = 0; k < mdlhdr.numlocalanim * mdlhdr.numbones; k++)
					{
						int dist = 0;
						int nextAnim = i + 1;
						int hdrStartPos = Stream->Position();
						//Logger::Info("secHdrStartPos: %d\n", hdrStartPos);
						mstudioanim_t_v53 anim; Stream->Read(&anim);
						sections.push_back(anim);
						mstudioanimdata_t_v49 animData;

						//Logger::Info("SecHdrNum: %d\n", j);
						//Logger::Info("SecAnimScale: %d\n", anim.animscale);
						//Logger::Info("SecBone: %d\n", anim.bone);
						//Logger::Info("SecFlags Read: %d\n", anim.flags);
						//Logger::Info("Unk Read: %d\n", anim.unk);
						//Logger::Info("SecRawRot Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot.x, anim.rawrot.y, anim.rawrot.z, anim.rawrot.wneg);
						//Logger::Info("SecRawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
						//Logger::Info("SecRawScale Read: X: %d Y: %d Z: %d\n", anim.rawscale.x, anim.rawscale.y, anim.rawscale.z);
						//Logger::Info("SecRotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
						//Logger::Info("SecPosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);
						//Logger::Info("SecScalePtr Read: X: %d Y: %d Z: %d\n", anim.animscale.offset.x, anim.animscale.offset.y, anim.animscale.offset.z);
						//Logger::Info("SecNextOffset Read: %d\n", anim.nextoffset);

						int headerSize = 32;
						int hdrEndPos = Stream->Position();
						//Logger::Info("secHdrEndPos: %d\n", hdrEndPos);
						secBoneHeaderNum++;
						if (anim.nextoffset == 0)
						{
							if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - hdrEndPos;  //Logger::Info("Distance from sectionIdx: %d\n", strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - hdrEndPos;
							if (nextSection < secNumber && nextAnim > mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + idxs[nextSection].sectionoffsets - hdrEndPos;
							if (nextSection > secNumber && nextAnim > mdlhdr.numlocalanim && animdescs[i].numikrules == 0) dist = mdlhdr.localseqindex - hdrEndPos;
							Stream->Read(&animData, dist);
							//Logger::Info("AnimDataSizeEnd: %d\n", dist);
							Logger::Info("Section Read: %d\n", i);
							break;
						}
						dist = anim.nextoffset - headerSize;
						Stream->Read(&animData, dist);
					}

				}

			}
	#pragma endregion
	#pragma region IKRULES
			if (animdescs[i].numikrules > 0)
			{
				int numOfCompressedIkErrors = 0;
				int numOfIkErrors = 0;
				std::vector<int> errorIdxs;
				int nextAnim = i + 1;

				for (int j = 0; j < animdescs[i].numikrules; j++)
				{
					Stream->seek(animDescPos + animdescs[i].ikruleindex + 140 * j);
					int pos = Stream->Position();
					mstudioikrule_t_v53 v53IkRule; Stream->Read(&v53IkRule);
					ikrules.push_back(v53IkRule);
					if (v53IkRule.compressedikerrorindex > 0) errorIdxs.push_back(pos + v53IkRule.compressedikerrorindex);
					if (v53IkRule.compressedikerrorindex > 0) numOfCompressedIkErrors++;
					if (v53IkRule.ikerrorindex > 0) numOfIkErrors++;

					Logger::Info("IkRule Read: %d\n", j);
					numikrules++;
				}
	#pragma endregion
	#pragma region COMPRESSEDIKERRORS
				if (numOfCompressedIkErrors > 0)
				{
					int animStartPos = mdlhdr.localanimindex + 92 * i;
					for (int j = 0; j < numOfCompressedIkErrors; j++)
					{
						Stream->seek(errorIdxs[j]);
						int dist = 0;
						int next = j + 1;
						int pos = Stream->Position();
						mstudiocompressedikerror_t_v49 v49CompressedIkError; Stream->Read(&v49CompressedIkError);
						compressedikerrors.push_back(v49CompressedIkError);
						mstudioanimdata_t_v49 animData;
						int endPos = Stream->Position();
						if (j + 1 > numOfCompressedIkErrors - 1)
						{
							if (animdescs[i].compressedikerrorindex > 0) dist = animDescPos + animdescs[i].compressedikerrorindex - endPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0)  dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - endPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - endPos;
							if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - endPos;
							Stream->Read(&animData, dist);
							Logger::Info("CompressedIkError Read: %d\n", j);
							break;
						}
						Logger::Info("CompressedIkError Read: %d\n", j);
						dist = errorIdxs[next] - endPos;
						Stream->Read(&animData, dist);
					}
				}
	#pragma endregion
	#pragma region V52COMPRESSEDERROR
				if (animdescs[i].compressedikerrorindex > 0)
				{
					int animStartPos = mdlhdr.localanimindex + 92 * i;
					Stream->seek(animStartPos + animdescs[i].compressedikerrorindex);
					int dist = 0;
					int pos = Stream->Position();
					mstudiocompressedikerror_t_v52 v52CompressedIkError; Stream->Read(&v52CompressedIkError);
					v52compressedikerrors.push_back(v52CompressedIkError);
					mstudioanimdata_t_v49 animData;
					int endPos = Stream->Position();
					if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0)  dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - endPos;
					if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - endPos;
					if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - endPos;
					Stream->Read(&animData, dist);
					Logger::Info("v52CompressedIkError Read: %d\n");
				}

			}

		}
	}
	#pragma endregion
	#pragma region SEQDESCS
	if (mdlhdr.numlocalseq > 0)
	{
		#pragma region SEQDESC
		Stream->seek(mdlhdr.localseqindex);
		for (int i = 0; i < mdlhdr.numlocalseq; i++)
		{
			int seqPos = mdlhdr.localseqindex + 232 * i;
			Stream->seek(seqPos);
			mstudioseqdesc_t_v53 seqDesc; Stream->Read(&seqDesc);
			seqDesc.szlabel = Stream->ReadNullTermStrTrgt(seqPos + seqDesc.szlabelindex, false);
			seqDesc.szactivityname = Stream->ReadNullTermStrTrgt(seqPos + seqDesc.szactivitynameindex, false);//ReadMDLString(Stream, seqPos + seqDesc.szactivitynameindex);

			seqdescs.push_back(seqDesc);

			Logger::Info("Seq Read: %s : %s : %d \n", seqDesc.szlabel.c_str(), seqDesc.szactivityname.c_str(), i);
		}
		#pragma endregion
		#pragma region SEQDATA
		for (int i = 0; i < mdlhdr.numlocalseq; i++)
		{
			#pragma region VARS
			int seqPos = mdlhdr.localseqindex + 232 * i;
			int numBlends = seqdescs[i].groupsize[0] * seqdescs[i].groupsize[1];
			int posekeySize = seqdescs[i].groupsize[0] + seqdescs[i].groupsize[1];
			int numAutolayers = seqdescs[i].numautolayers;
			int numActMods = seqdescs[i].numactivitymodifiers;
			int numEvents = seqdescs[i].numevents;
			#pragma endregion
			#pragma region WEIGHTLIST
			if (seqdescs[i].weightlistindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].weightlistindex);
				seqweightlist_t_v49 weightlist; Stream->Read(&weightlist, mdlhdr.numbones);
				seqweightlist.push_back(weightlist);
				Logger::Info("Weightlist Read: %d\n", i);
			}
			#pragma endregion
			#pragma region BLENDS
			if (seqdescs[i].animindexindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].animindexindex);
				blendgroup_t_v49 blendgroup; Stream->Read(&blendgroup, numBlends);
				seqdescs[i].blends = blendgroup;
				blends.push_back(blendgroup);
				Logger::Info("BlendGroup Read: %d\n", i);
			}
			#pragma endregion
			#pragma region POSEKEYS
			if (seqdescs[i].posekeyindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].posekeyindex);
				posekey_t_v49 posekey; Stream->Read(&posekey, posekeySize);
				if (posekeySize == 3)posekey.unk[2] == 0.0;
				//				Logger::Info("PosekeyF Read: %f\n", posekey.unk[2]);
				posekeys.push_back(posekey);
				Logger::Info("Posekey Read: %d\n", i);
			}
			#pragma endregion
			#pragma region AUTOLAYERS
			if (numAutolayers > 0)
			{
				Stream->seek(seqPos + seqdescs[i].autolayerindex);
				for (int j = 0; j < numAutolayers; j++)
				{
					mstudioautolayer_t_v49 autolayer; Stream->Read(&autolayer);
					autolayers.push_back(autolayer);
					Logger::Info("Autolayer Read: %d\n", j);
				}
			}
			#pragma endregion
			#pragma region EVENTS
			if (numEvents > 0)
			{
				Stream->seek(seqPos + seqdescs[i].eventindex);
				for (int j = 0; j < seqdescs[i].numevents; j++)
				{
					mstudioevent_t_v49 _event; Stream->Read(&_event);
					events.push_back(_event);

					std::string szEventName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].eventindex + 80 * j + _event.szeventindex, false);
					std::string szName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].eventindex + 80 * j + 12, false);

					seqdescs[i].szeventnames.push_back(szEventName);

					Logger::Info("Event Read: %s : %s : %d\n", szName.c_str(), szEventName.c_str(), j);
				}
			}
			#pragma endregion
			#pragma region ACTIVITYMODIFIERS
			if (numActMods > 0)
			{
				Stream->seek(seqPos + seqdescs[i].activitymodifierindex);
				for (int j = 0; j < seqdescs[i].numactivitymodifiers; j++)
				{
					mstudioactivitymodifier_t_v53 actmod; Stream->Read(&actmod);

					std::string szName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].activitymodifierindex + 8 * j + actmod.sznameindex, false);

					seqdescs[i].szactivitymodifiernames.push_back(szName);
					actmod.szname = szName;
					activitymodifiers.push_back(actmod);
					Logger::Info("ActMod Read: %s : %d\n", seqdescs[i].szactivitymodifiernames[j].c_str(), i);
				}
			}
			#pragma endregion
		}
		#pragma endregion
	}
	#pragma endregion
	#pragma region NODES
	if (mdlhdr.numlocalnodes > 0)
	{
		#pragma region NODENAMES
				Stream->seek(mdlhdr.localnodenameindex);
				for (int i = 0; i < mdlhdr.numlocalnodes; i++)
				{
					mstudionodename_t_v49 nodeName; Stream->Read(&nodeName);

					std::string szName = Stream->ReadNullTermStrTrgt(nodeName.sznameindex, false);
					nodeName.szname = szName;

					nodenames.push_back(nodeName);
					Logger::Info("NodeName Read: %s\n", szName.c_str());
					Logger::Info("NodeName Read: %d\n", i);
				}
		#pragma endregion
		#pragma region NODEDATA
		Stream->seek(mdlhdr.localnodeindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodedata_v49 node; Stream->Read(&node, mdlhdr.numlocalnodes * mdlhdr.numlocalnodes);
			nodes.push_back(node);
			Logger::Info("Node Read: %d\n", i);
		}
#pragma endregion
	}
	#pragma endregion
	#pragma region BODYPARTS
	if (mdlhdr.numbodyparts > 0)
	{
		Stream->seek(mdlhdr.bodypartindex);
		for (int i = 0; i < mdlhdr.numbodyparts; i++)
		{
			mstudiobodyparts_t_v49 bodyPart; Stream->Read(&bodyPart);
			if (bodyPart.nummodels > 0) nummodels += bodyPart.nummodels;

			std::string szName = Stream->ReadNullTermStrTrgt(mdlhdr.bodypartindex + 16 * i + bodyPart.sznameindex, false);
			bodyPart.szname = szName;

			bodyparts.push_back(bodyPart);
			Logger::Info("Bodypart Read: %s\n", szName.c_str());
			Logger::Info("Bodypart Read: %d\n", i);
		}
	}
	#pragma endregion
	#pragma region MODELS
	if (nummodels > 0)
	{
		Stream->seek(mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts);
		for (int i = 0; i < nummodels; i++)
		{
			mstudiomodel_t_v49 model; Stream->Read(&model);
			if (model.nummeshes > 0) nummeshes += model.nummeshes;
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts + 148 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(modelPos, false);

			models.push_back(model);
			Logger::Info("Model Read: %s\n", szName.c_str());
			Logger::Info("Model Read: %d\n", i);
		}

	}
	#pragma endregion
	#pragma region IKCHAINS
	if (mdlhdr.numikchains > 0)
	{
		Stream->seek(mdlhdr.ikchainindex);
		for (int i = 0; i < mdlhdr.numikchains; i++)
		{
			mstudioikchain_t_v53 ikchain; Stream->Read(&ikchain);
			if (ikchain.numlinks > 0) numiklinks += ikchain.numlinks;
			int ikChainPos = mdlhdr.ikchainindex + 32 * i;
			std::string szName = Stream->ReadNullTermStrTrgt(ikChainPos + ikchain.sznameindex, false);
			ikchain.szname = szName;

			ikchains.push_back(ikchain);

			Logger::Info("IkChain Read: %s\n", szName.c_str());
			Logger::Info("IkChain Read: %d\n", i);
		}
	#pragma endregion
	#pragma region IKLINKS
		for (int i = 0; i < numiklinks; i++)
		{
			int ikLinkPos = mdlhdr.ikchainindex + 16 * mdlhdr.numikchains;
			Stream->seek(ikLinkPos);
			mstudioiklink_t_v49 iklink; Stream->Read(&iklink);
			iklinks.push_back(iklink);
			Logger::Info("IkLink Read: %d\n", i);
		}
	}
	#pragma endregion
	#pragma region POSEPARAMDESCS
	if (mdlhdr.localposeparamindex > 0)
	{
		Stream->seek(mdlhdr.localposeparamindex);
		for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
		{
			mstudioposeparamdesc_t_v49 poseparamdesc; Stream->Read(&poseparamdesc);
			int poseParamPos = mdlhdr.localposeparamindex + 20 * i;
			std::string szName = Stream->ReadNullTermStrTrgt(poseParamPos + poseparamdesc.sznameindex, false);
			poseparamdesc.szname = szName;

			poseparamdescs.push_back(poseparamdesc);



			Logger::Info("PoseParamDesc Read: %s\n", szName.c_str());
			Logger::Info("PoseParamDesc Read: %d\n", i);
		}
	}
	#pragma endregion
	#pragma region MESHES
	if (nummeshes > 0)
	{
		for (int i = 0; i < nummeshes; i++)
		{
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts;
			int meshPos = modelPos + 148 * nummodels + 20 * mdlhdr.numlocalposeparameters + 32 * mdlhdr.numikchains + 28 * numiklinks + 116 * i;
			Stream->seek(meshPos);
			mstudiomesh_t_v49 mesh; Stream->Read(&mesh);
			meshes.push_back(mesh);

			int texturePos = mdlhdr.textureindex + (44 * (int)mesh.material);
			Stream->seek(texturePos);
			mstudiotexture_t_v53 texture; Stream->Read(&texture);

			std::string szName = Stream->ReadNullTermStrTrgt(mdlhdr.textureindex + 44 * mesh.material + texture.sznameindex, false);

			Logger::Info("MeshMat Read: %s\n", szName.c_str());
			Logger::Info("Mesh Read: %d\n", mesh.material);
		}
	}
	#pragma endregion
	#pragma region INCLUDEMODEL
	if (mdlhdr.numincludemodels > 0)
	{
		Stream->seek(mdlhdr.includemodelindex);
		for (int i = 0; i < mdlhdr.numincludemodels; i++)
		{
			mstudiomodelgroup_t_v49 includemodel; Stream->Read(&includemodel);
			int includedModelPos = mdlhdr.includemodelindex + 8 * i;
			std::string szName = Stream->ReadNullTermStrTrgt(includedModelPos + includemodel.sznameindex, false);
			includemodel.szname = szName;

			includedmodels.push_back(includemodel);

			Logger::Info("IncludedModel Read: %s\n", ReadMDLString(Stream, includedModelPos + includedmodels[i].sznameindex).c_str());
			Logger::Info("IncludedModel Read: %d\n", i);
		}
	}
	#pragma endregion
	#pragma region TEXTURES
	//Logger::Info("Textureidx: %d\n", mdlhdr.textureindex);
	if (mdlhdr.numtextures > 0)
	{
		Stream->seek(mdlhdr.textureindex);
		for (int i = 0; i < numTextures; i++)
		{
			mstudiotexture_t_v53 texture; Stream->Read(&texture);
			int texturePos = mdlhdr.textureindex + 44 * i;
			std::string szName = Stream->ReadNullTermStrTrgt(texturePos + texture.sznameindex, false);
			texture.szname = szName;

			textures.push_back(texture);

			Logger::Info("Texture Read: %s\n", szName.c_str());
			Logger::Info("Texture Read: %d\n", i);
		}
	}
	#pragma endregion
	#pragma region CDMATERIALS
	if (mdlhdr.numcdtextures > 0)
	{
		Stream->seek(mdlhdr.cdtextureindex);
		for (int i = 0; i < mdlhdr.numcdtextures; i++)
		{
			mstudiotexturedir_t_v49 cdtexture; Stream->Read(&cdtexture);

			std::string szName = Stream->ReadNullTermStrTrgt(cdtexture.sznameindex, false);
			cdtexture.szname = szName;

			cdtextures.push_back(cdtexture);

			Logger::Info("CdTexture Read: %s\n", szName.c_str());
			Logger::Info("CdTexture Read: %d\n", i);
		}
	}
	#pragma endregion
	#pragma region SKINFAMILIES
	if (mdlhdr.numskinfamilies > 0)
	{
		if (mdlhdr.numskinref > 0)
		{
			Stream->seek(mdlhdr.skinindex);
			for (int i = 0; i < mdlhdr.numskinfamilies; i++)
			{
				mstudioskingroup_t_v49 skingroup; Stream->Read(&skingroup, mdlhdr.numskinref);
				skingroups.push_back(skingroup);
				Logger::Info("SkinGroup Read: %d\n", i);
			}
		}

	}
	#pragma endregion
	#pragma region KEYVALUES
	if (mdlhdr.keyvaluesize > 0)
	{
		Stream->seek(mdlhdr.keyvalueindex);
		Stream->Read(&keyvalues, mdlhdr.keyvaluesize);
		Logger::Info("Keyvalues Read: %d\n", mdlhdr.keyvaluesize);
	}
	#pragma endregion
	#pragma region SRCBONETRANSFORMS
	if (mdlhdr.numsrcbonetransform > 0)
	{
		Stream->seek(mdlhdr.srcbonetransformindex);
		for (int i = 0; i < mdlhdr.numsrcbonetransform; i++)
		{
			mstudiosrcbonetransform_t_v49 srcbonetransform; Stream->Read(&srcbonetransform);

			int srcBonePos = mdlhdr.srcbonetransformindex + 100 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(srcBonePos + srcbonetransform.sznameindex,false);
			srcbonetransform.szname = szName;

			srcbonetransforms.push_back(srcbonetransform);

			Logger::Info("SrcBoneTransform Read: %s\n", szName.c_str());
			Logger::Info("SrcBoneTransform Read: %d\n", i);
		}
	}
	#pragma endregion
	#pragma region LINEARBONE
	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlhdr.linearboneindex);
		Stream->Read(&linearbone);
		Stream->Read(&linearbonedata, mdlhdr.numbones);
		Logger::Info("LinearBone Read: %d\n", mdlhdr.numbones);
	}
	#pragma endregion
	#pragma region PERTRIHDR
//	Logger::Info("Test\n");
	if (mdlhdr.aabbindex > 0)
	{
		Stream->seek(mdlhdr.aabbindex);
		Stream->Read(&pertriheader);
		Logger::Info("AABB Per Triangle Header Read:\n");
	}
	#pragma endregion
	#pragma region AABBNODES
	if (mdlhdr.numaabbnodes > 0)
	{
		for (int i = 0; i < mdlhdr.numaabbnodes; i++)
		{
			aabbnode _aabbnode;
			Stream->Read(&_aabbnode);
			aabbnodes.push_back(_aabbnode);
		}
//		Logger::Info("Pos Read: %d\n", Stream->Position());
		Logger::Info("AABB Nodes Read: %d\n", mdlhdr.numaabbnodes);
	}
	#pragma endregion
	#pragma region AABBLEAVES
	if (mdlhdr.numaabbleaves > 0)
	{

		for (int i = 0; i < mdlhdr.numaabbleaves; i++)
		{
			aabbleaf _aabbleaf;
			Stream->Read(&_aabbleaf);
			aabbleaves.push_back(_aabbleaf);
		}
		Logger::Info("AABB Leaves Read: %d\n", mdlhdr.numaabbleaves);
	}
	#pragma endregion
	#pragma region AABBVERTS
	if (mdlhdr.numaabbverts > 0)
	{
		Stream->Read(&aabbverts, mdlhdr.numaabbverts);
		Logger::Info("Pos Read: %d\n", Stream->Position());
		Logger::Info("AABB Verts Read: %d\n", mdlhdr.numaabbverts);
	}
	#pragma endregion
	#pragma region STRINGTABLE
	Stream->seek(mdlhdr.sznameindex);
	mstudiostringtable_t_v52 stringTable{}; Stream->Read(&stringTable, mdlhdr, seqdescs, hitboxsets, attachments, nodenames, bodyparts, ikchains, animdescs, textures, includedmodels, cdtextures, poseparamdescs, srcbonetransforms);
	#pragma endregion
	#pragma region RUIMESH
	if (mdlhdr.numruimeshes > 0)
	{
		Stream->seek(mdlhdr.ruimeshindex);
		#pragma region HDR
		for (int i = 0; i < mdlhdr.numruimeshes; i++)
		{
			mstudiorruiheader_t ruiHdr; Stream->Read(&ruiHdr);
			ruiHdrs.push_back(ruiHdr);
			Logger::Info("RuiHdr Read: %d\n", i);
		}
		#pragma endregion
		#pragma region MESH
				Stream->seek(mdlhdr.ruimeshindex + ruiHdrs[0].ruimeshindex);
				for (int j = 0; j < ruiHdrs.size(); j++)
				{
					int ruiPos = Stream->Position();
					mstudioruimesh_t ruiMesh; Stream->Read(&ruiMesh);
					ruiMeshes.push_back(ruiMesh);
					RUIMeshToSmd(ruiMesh);

					std::string szName = Stream->ReadNullTermStrTrgt(ruiPos + 32, false);

					Logger::Info("RuiMesh Read: %s\n", szName.c_str());
					Logger::Info("RuiMesh Read: %d\n", j);
				}
		#pragma endregion
//		Logger::Info("RuiMesh5 Read: %s\n", Stream->_filename);
//		std::string fileName = Stream->_filename;
//		fileName.resize(fileName.size() - 4);
//		fileName.insert(fileName.size(), ".rui");
//		int ruiEnd = Stream->Position();
//		Stream->seek(mdlhdr.ruimeshindex);
//		BinaryWriter Bing = BinaryWriter(fileName.c_str());
//		char* data = new char[ruiEnd - mdlhdr.ruimeshindex];
//		Stream->read(data, ruiEnd - mdlhdr.ruimeshindex);
//		Bing.write(data, ruiEnd - mdlhdr.ruimeshindex);
	}
	#pragma endregion

	MDL::v53Mdl _v53mdl{ mdlhdr,mayabakery,bones,jigglebones,boneflexdrivers,attachments,hitboxsets,hitboxes,bonenametable,animdescs,anims, sectionindexes, sections, ikrules,compressedikerrors,v52compressedikerrors,ikerrors,ikrulezeroframe,seqdescs,blends,posekeys,events,autolayers,activitymodifiers,seqweightlist,nodenames,nodes,bodyparts,models,meshes,ikchains,iklinks,poseparamdescs,includedmodels,ruiHdrs,ruiMeshes,textures,cdtextures,skingroups,keyvalues,srcbonetransforms, linearbone,linearbonedata, stringTable, pertriheader, aabbnodes, aabbleaves, aabbverts };
	return _v53mdl;
}

#pragma endregion

#pragma region MDLV52

MDL::v52Mdl::v52Mdl()
{

}

MDL::v52Mdl::v52Mdl(studiohdr_t_v52 _mdlhdr, studiohdr2_t_v52 _mdlsubhdr, mayabakery_t _mayabakery, std::vector<mstudiobone_t_v52> _bones, std::vector<mstudiojigglebone_t_v49> _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, std::vector<mstudioattachment_t_v49> _attachments, std::vector<mstudiohitboxset_t_v49> _hitboxsets, std::vector<mstudiobbox_t_v49> _hitboxes, mstudiobonenametable_t_v49 _bonenametable, std::vector<mstudioanimdesc_t_v52> _animdescs, std::vector<mstudioanim_t_v49> _anims, std::vector<sectionindexesindex_t_v49> _sectionindexes, std::vector<mstudioanim_t_v49> _sections, std::vector<mstudioikrule_t_v49> _ikrules, std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors, std::vector<mstudiocompressedikerror_t_v52>	_v52compressedikerrors, std::vector<mstudioikerror_t_v49> _ikerrors, std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe, std::vector<mstudioseqdesc_t_v52> _seqdescs, std::vector<blendgroup_t_v49> _blends, std::vector<posekey_t_v49> _posekeys, std::vector<mstudioevent_t_v49> _events, std::vector<mstudioautolayer_t_v49> _autolayers, std::vector<mstudioactivitymodifier_t_v53> _activitymodifiers, std::vector<seqweightlist_t_v49> _seqweightlist, std::vector<mstudionodename_t_v49> _nodenames, std::vector<mstudionodedata_v49> _nodes, std::vector<mstudiobodyparts_t_v49> _bodyparts, std::vector<mstudiomodel_t_v49>	_models, std::vector<mstudiomesh_t_v49> _meshes, std::vector<mstudioikchain_t_v49> _ikchains, std::vector<mstudioiklink_t_v49> _iklinks, std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs, std::vector<mstudioanimblock_t> _animblocks, std::vector<mstudiomodelgroup_t_v49> _includedmodels, std::vector<mstudiotexturedir_t_v49> _cdtextures, std::vector<mstudiotexture_t_v49> _textures, std::vector<mstudioskingroup_t_v49> _skingroups, mstudiokeyvalues_t_v49 _keyvalues, std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v49 _linearbonedata, mstudiostringtable_t_v52 _stringtable, pertriheader_t _pertriheader, std::vector<aabbnode> _aabbnodes, std::vector<aabbleaf> _aabbleaves, aabbcollisionmask _aabbverts)
{
	mdlhdr = _mdlhdr;
	mdlsubhdr = _mdlsubhdr;
	mayabakery = _mayabakery;
	bones = _bones;
	jigglebones = _jigglebones;
	boneflexdrivers = _boneflexdrivers;
	attachments = _attachments;
	hitboxsets = _hitboxsets;
	hitboxes = _hitboxes;
	bonenametable = _bonenametable;
	animdescs = _animdescs;
	anims = _anims;
	sectionindexes = _sectionindexes;
	sections = _sections;
	ikrules = _ikrules;
	compressedikerrors = _compressedikerrors;
	v52compressedikerrors = _v52compressedikerrors;
	ikerrors = _ikerrors;
	ikrulezeroframe = _ikrulezeroframe;
	seqdescs = _seqdescs;
	blends = _blends;
	posekeys = _posekeys;
	events = _events;
	autolayers = _autolayers;
	activitymodifiers = _activitymodifiers;
	seqweightlist = _seqweightlist;
	nodenames = _nodenames;
	nodes = _nodes;
	bodyparts = _bodyparts;
	models = _models;
	meshes = _meshes;
	ikchains = _ikchains;
	iklinks = _iklinks;
	poseparamdescs = _poseparamdescs;
	animblocks = _animblocks;
	includedmodels = _includedmodels;
	cdtextures = _cdtextures;
	textures = _textures;
	skingroups = _skingroups;
	keyvalues = _keyvalues;
	srcbonetransforms = _srcbonetransforms;
	linearbone = _linearbone;
	linearbonedata = _linearbonedata;
	stringtable = _stringtable;
	pertriheader = _pertriheader;
	aabbnodes = _aabbnodes;
	aabbleaves = _aabbleaves;
	aabbverts = _aabbverts;
}

void MDL::v52Mdl::v52ExtractSrcBoneTransforms(BinaryReader* Stream)
{
	if (mdlsubhdr.numsrcbonetransform > 0)
	{
		std::string fileName = Stream->_filename;
		fileName.resize(fileName.size() - 4);
		fileName.insert(fileName.size(), ".srcbtf");
		BinaryWriter Bing = BinaryWriter(fileName.c_str());

		for (int i = 0; i < mdlsubhdr.numsrcbonetransform; i++)
		{
			Bing.Write(srcbonetransforms[i]);
		}
	}
}

void MDL::v52Mdl::v52ExtractAABBTree(BinaryReader* Stream)
{
	if (mdlsubhdr.aabbindex > 0)
	{
		std::string fileName = Stream->_filename;
		fileName.resize(fileName.size() - 4);
		fileName.insert(fileName.size(), ".aabb");
		Stream->seek(mdlsubhdr.aabbindex);
		BinaryWriter Bing = BinaryWriter(fileName.c_str());
		Stream->Read(&pertriheader);
		Bing.Write(pertriheader);

		if (mdlsubhdr.numaabbnodes > 0)
		{
			for (int i = 0; i < mdlsubhdr.numaabbnodes; i++)
			{
				Bing.Write(aabbnodes[i]);
			}
			//		Logger::Info("Pos Read: %d\n", Stream->Position());
		}

		if (mdlsubhdr.numaabbleaves > 0)
		{
			for (int i = 0; i < mdlsubhdr.numaabbleaves; i++)
			{
				Bing.Write(aabbleaves[i]);
			}
		}

		if (mdlsubhdr.numaabbverts > 0)
		{
			Bing.Write(aabbverts);
		}

	}
}

MDL::v52Mdl MDL::v52Mdl::_v52Mdl(BinaryReader* Stream, bool debug)
{
#pragma region HDR
	studiohdr_t_v52	 mdlhdr; Stream->Read(&mdlhdr);
#pragma endregion
#pragma region SUBHDR
	studiohdr2_t_v52 mdlsubhdr; Stream->Read(&mdlsubhdr);
#pragma endregion
#pragma region VARS
	mayabakery_t										mayabakery{};
	std::vector<mstudiobone_t_v52>						bones{};
	std::vector<mstudiojigglebone_t_v49>				jigglebones{};
	std::vector<mstudioboneflexdriver_t_v49>			boneflexdrivers{};
	std::vector<mstudioattachment_t_v49>				attachments{};
	std::vector<mstudiohitboxset_t_v49>					hitboxsets{};
	std::vector<mstudiobbox_t_v49>						hitboxes{};
	mstudiobonenametable_t_v49							bonenametable{};
	std::vector<mstudioanimdesc_t_v52>					animdescs{};
	std::vector<mstudioanim_t_v49>						anims{};
	std::vector<sectionindexesindex_t_v49>				sectionindexes{};
	std::vector<mstudioanim_t_v49>						sections{};
	std::vector<mstudioikrule_t_v49>					ikrules{};
	std::vector<mstudiocompressedikerror_t_v49>			compressedikerrors{};
	std::vector<mstudiocompressedikerror_t_v52>			v52compressedikerrors{};
	std::vector<mstudioikerror_t_v49>					ikerrors{};
	std::vector<mstudioikrulezeroframe_t_v49>			ikrulezeroframe{};
	std::vector<mstudioseqdesc_t_v52>					seqdescs{};
	std::vector<blendgroup_t_v49>						blends{};
	std::vector<posekey_t_v49>							posekeys{};
	std::vector<mstudioevent_t_v49>						events{};
	std::vector<mstudioautolayer_t_v49>					autolayers{};
	std::vector<mstudioactivitymodifier_t_v53>			activitymodifiers{};
	std::vector<seqweightlist_t_v49>					seqweightlist{};
	std::vector<mstudionodename_t_v49>					nodenames{};
	std::vector<mstudionodedata_v49>					nodes{};
	std::vector<mstudiobodyparts_t_v49>					bodyparts{};
	std::vector<mstudiomodel_t_v49>						models{};
	std::vector<mstudiomesh_t_v49>						meshes{};
	std::vector<mstudioikchain_t_v49>					ikchains{};
	std::vector<mstudioiklink_t_v49>					iklinks{};
	std::vector<mstudioposeparamdesc_t_v49>				poseparamdescs{};
	std::vector<mstudioanimblock_t>						animblocks{};
	std::vector<mstudiomodelgroup_t_v49>				includedmodels{};
	std::vector<mstudiotexturedir_t_v49>				cdtextures{};
	std::vector<mstudiotexture_t_v49>					textures{};
	std::vector<mstudioskingroup_t_v49>					skingroups{};
	mstudiokeyvalues_t_v49								keyvalues{};
	std::vector<mstudiosrcbonetransform_t_v49>			srcbonetransforms{};
	mstudiolinearbone_t_v49								linearbone{};
	mstudiolinearbonedata_t_v49							linearbonedata{};
	pertriheader_t										pertriheader{};
	std::vector<aabbnode>								aabbnodes{};
	std::vector<aabbleaf>								aabbleaves{};
	aabbcollisionmask									aabbverts{};
	mstudiostringtable_t_v52							stringTable{};

	int numjigglebones = 0;

	int numhitboxes = 0;

	int nummodels = 0;

	int nummeshes = 0;

	int numiklinks = 0;

	int numiklocks = 0;

	int numikrules = 0;

#pragma endregion
#pragma region MAYABAKERY
	if (mdlhdr.boneindex > mdlhdr.mayaindex && mdlhdr.mayaindex != 0)
	{
		Stream->seek(mdlhdr.mayaindex);
		int mayaBakerySize = mdlhdr.boneindex - mdlhdr.mayaindex;
		Stream->Read(&mayabakery, mayaBakerySize);
		Logger::Info("MayaBakery Read: \n");
	}
#pragma endregion
#pragma region BONES
	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlhdr.boneindex);
		for (int i = 0; i < mdlhdr.numbones; i++)
		{
			int bonePos = mdlhdr.boneindex + 216 * i;
			Stream->seek(bonePos);

			mstudiobone_t_v52 bone; Stream->Read(&bone);
			bones.push_back(bone);
			if (bone.proctype == 5) numjigglebones++;

			std::string szName = Stream->ReadNullTermStrTrgt(bonePos + bone.sznameindex, false);

			Logger::Info("Bone Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region JIGGLEBONES
	if (numjigglebones > 0)
	{
		int jiggleBonePos = mdlhdr.boneindex + 216 * mdlhdr.numbones;

		Stream->seek(jiggleBonePos);

		for (int i = 0; i < numjigglebones; i++)
		{
			mstudiojigglebone_t_v49 jigglebone; Stream->Read(&jigglebone);
			jigglebones.push_back(jigglebone);
			Logger::Info("JiggleBone Read: %d\n", i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
#pragma endregion
#pragma region ATTACHMENTS
	if (mdlhdr.numlocalattachments > 0)
	{
		Stream->seek(mdlhdr.localattachmentindex);
		for (int i = 0; i < mdlhdr.numlocalattachments; i++)
		{
			int attachmentPos = mdlhdr.localattachmentindex + 92 * i;
			Stream->seek(attachmentPos);

			mstudioattachment_t_v49 attachment; Stream->Read(&attachment);

			std::string szName = Stream->ReadNullTermStrTrgt(attachmentPos + attachment.sznameindex, false);
			attachment.szname = szName;

			attachments.push_back(attachment);
			Logger::Info("Attachment Read: %s : %d\n", attachment.szname.c_str(), i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
#pragma endregion
#pragma region HITBOXSETS
	if (mdlhdr.numhitboxsets > 0)
	{
		Stream->seek(mdlhdr.hitboxsetindex);
		for (int i = 0; i < mdlhdr.numhitboxsets; i++)
		{
			int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * i;
			Stream->seek(hitboxsetPos);

			mstudiohitboxset_t_v49 hitboxset; Stream->Read(&hitboxset);

			if (hitboxset.numhitboxes > 0) numhitboxes += hitboxset.numhitboxes;

			Stream->seek(hitboxsetPos + hitboxset.hitboxindex);
			for (int j = 0; j < hitboxset.numhitboxes; j++)
			{
				int hitboxPos = hitboxsetPos + hitboxset.hitboxindex + 68 * i;
				Stream->seek(hitboxPos);
				mstudiobbox_t_v49 hitbox; Stream->Read(&hitbox);

				std::string szHitboxName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.szhitboxnameindex, false);
				hitbox.szhitboxname = szHitboxName;

				hitboxset.hitboxes.push_back(hitbox);
			}

			std::string szName = Stream->ReadNullTermStrTrgt(hitboxsetPos + hitboxset.sznameindex, false);
			hitboxset.szname = szName;

			hitboxsets.push_back(hitboxset);
			Logger::Info("HitboxSet Read: %s : %d\n", szName.c_str(), i);
		}
#pragma endregion
#pragma region HITBOXES
		if (numhitboxes > 0)
		{
			for (int i = 0; i < numhitboxes; i++)
			{
				int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * mdlhdr.numhitboxsets;
				int hitboxPos = hitboxsetPos + 68 * i;
				Stream->seek(hitboxPos);
				mstudiobbox_t_v49 hitbox; Stream->Read(&hitbox);

				std::string szName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.szhitboxnameindex, false);
				hitbox.szhitboxname = szName;

				hitboxes.push_back(hitbox);
				Logger::Info("Hitbox Read: %s : %d\n", hitbox.szhitboxname.c_str(), i);
			}
		}
		//Logger::Info("strPos: %d\n", Stream->Position());

	}
#pragma endregion
#pragma region BONENAMETABLE
	if (mdlhdr.numbones > 0)
	{
		Stream->Read(&bonenametable, mdlhdr.numbones);
		Logger::Info("BoneTable Read: %d\n", mdlhdr.numbones);
	}
#pragma endregion
#pragma region ANIMDESCS
	if (mdlhdr.numlocalanim > 0)
	{
		Stream->seek(mdlhdr.localanimindex);
		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int animDescPos = mdlhdr.localanimindex + 100 * i;
			Stream->seek(animDescPos);

			mstudioanimdesc_t_v52 animdesc; Stream->Read(&animdesc);

			std::string szName = Stream->ReadNullTermStrTrgt(animDescPos + animdesc.sznameindex, false);
			animdesc.szname = szName;

			animdescs.push_back(animdesc);

			Logger::Info("AnimDesc Read: %s : %d\n", szName.c_str(), i);
			//Logger::Info("strPos: %d\n", Stream->Position());
		}
#pragma endregion
#pragma region ANIMS

		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int boneHeaderNum = 0;
			int animDescPos = mdlhdr.localanimindex + 100 * i;
			int animPos = animDescPos + animdescs[i].animindex;

			if (animdescs[i].sectionindex == 0)
			{

				Stream->seek(animPos);
				//Logger::Info("strPos: %d\n", Stream->Position());
				for (int j = 0; j < mdlhdr.numlocalanim * mdlhdr.numbones; j++)
				{
					int dist = 0;
					int nextAnim = i + 1;
					int hdrStartPos = Stream->Position();
					//Logger::Info("hdrStartPos: %d\n", hdrStartPos);
					mstudioanim_t_v49 anim; Stream->Read(&anim);
					mstudioanimdata_t_v49 animData;

					//Logger::Info("HdrNum: %d\n", j);
					//Logger::Info("Bone: %d\n", anim.bone);
					//Logger::Info("Flags Read: %d\n", anim.flags);
					//Logger::Info("NextOffset Read: %d\n", anim.nextoffset);
					//Logger::Info("RawRot Read: One: %d I: %d J: %d K: %d\n", anim.rawrot.one, anim.rawrot.i, anim.rawrot.j, anim.rawrot.k);
					//Logger::Info("RawRot2 Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot2.x, anim.rawrot2.y, anim.rawrot2.z, anim.rawrot2.wneg);
					//Logger::Info("RawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
					//Logger::Info("RotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
					//Logger::Info("PosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);

					int headerSize = GetAnimHeaderSize((int)anim.flags);
					int hdrEndPos = Stream->Position();
					//Logger::Info("hdrEndPos: %d\n", hdrEndPos);
					boneHeaderNum++;
					if (anim.nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - hdrEndPos;
						if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - hdrEndPos;

						//Logger::Info("SeqIndx: %d\n", mdlhdr.localseqindex);
						//Logger::Info("AnimDataSizeEnd: %d\n", dist);
						Stream->Read(&animData, dist);
						anim.animdata = animData;
						anims.push_back(anim);
						//Logger::Info("Anim Read: %d\n", i);
						break;
					}
					dist = anim.nextoffset - headerSize;
					//Logger::Info("AnimDataSize: %d\n", dist);
					Stream->Read(&animData, dist);
					anim.animdata = animData;
					anims.push_back(anim);

				}
				Logger::Info("Anim Read: %d\n", i);

			}
#pragma endregion
#pragma region SECTIONS
			if (animdescs[i].sectionindex > 0)
			{
				int secBoneHeaderNum = 0;
				int animDescPos = mdlhdr.localanimindex + 100 * i;
				int animPos = animDescPos + animdescs[i].animindex;
				int secPos = animDescPos + animdescs[i].sectionindex;
				int frames = animdescs[i].numframes;
				int secFrames = animdescs[i].sectionframes;
				std::vector<sectionindexesindex_t_v49> idxs;

				int secNumber = (frames / secFrames) + 2;

				Stream->seek(secPos);

				for (int j = 0; j < secNumber; j++)
				{

					sectionindexesindex_t_v49 sectionIdx; Stream->Read(&sectionIdx);
					//Logger::Info("secIdx: %d\n", sectionIdx.sectionoffset);
					idxs.push_back(sectionIdx);
					sectionindexes.push_back(sectionIdx);
				}

				for (int j = 0; j < secNumber; j++)
				{
					sectionindexesindex_t_v49 sectionIdx = idxs[j];
					int nextSection = j + 1;
					//Logger::Info("secIdxPos: %d\n", animDescPos + sectionIdx.sectionoffset);
					Stream->seek(animDescPos + sectionIdx.sectionoffset);


					for (int k = 0; k < mdlhdr.numlocalanim * mdlhdr.numbones; k++)
					{
						int dist = 0;
						int nextAnim = i + 1;
						int hdrStartPos = Stream->Position();
						//Logger::Info("secHdrStartPos: %d\n", hdrStartPos);
						mstudioanim_t_v49 anim; Stream->Read(&anim);
						mstudioanimdata_t_v49 animData;

						//Logger::Info("SecHdrNum: %d\n", j);
						//Logger::Info("SecBone: %d\n", anim.bone);
						//Logger::Info("SecFlags Read: %d\n", anim.flags);
						//Logger::Info("SecNextOffset Read: %d\n", anim.nextoffset);
						//Logger::Info("SecRawRot Read: One: %d I: %d J: %d K: %d\n", anim.rawrot.one, anim.rawrot.i, anim.rawrot.j, anim.rawrot.k);
						//Logger::Info("SecRawRot2 Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot2.x, anim.rawrot2.y, anim.rawrot2.z, anim.rawrot2.wneg);
						//Logger::Info("SecRawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
						//Logger::Info("SecRotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
						//Logger::Info("SecPosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);

						int headerSize = GetAnimHeaderSize((int)anim.flags);
						int hdrEndPos = Stream->Position();
						//Logger::Info("secHdrEndPos: %d\n", hdrEndPos);
						secBoneHeaderNum++;
						if (anim.nextoffset == 0)
						{
							if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - hdrEndPos;  //Logger::Info("Distance from sectionIdx: %d\n", strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - hdrEndPos;
							if (nextSection < secNumber && nextAnim > mdlhdr.numlocalanim - 1 && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + idxs[nextSection].sectionoffset - hdrEndPos;
							if (nextSection > secNumber && nextAnim > mdlhdr.numlocalanim - 1 && animdescs[i].numikrules == 0) dist = mdlhdr.localseqindex - hdrEndPos;
							Stream->Read(&animData, dist);
							anim.animdata = animData;
							sections.push_back(anim);
							//Logger::Info("AnimDataSizeEnd: %d\n", dist);
							//Logger::Info("Section Read: %d\n", i);
							break;
						}
						dist = anim.nextoffset - headerSize;
						Stream->Read(&animData, dist);
						anim.animdata = animData;
						sections.push_back(anim);
					}

				}
				Logger::Info("Section Read: %d\n", i);

			}
#pragma endregion
#pragma region IKRULES
			if (animdescs[i].numikrules > 0)
			{
				int numOfCompressedIkErrors = 0;
				int numOfIkErrors = 0;
				std::vector<int> errorIdxs;
				int nextAnim = i + 1;

				for (int j = 0; j < animdescs[i].numikrules; j++)
				{
					Stream->seek(animDescPos + animdescs[i].ikruleindex + 152 * j);
					int pos = Stream->Position();
					mstudioikrule_t_v49 v49IkRule; Stream->Read(&v49IkRule);
					ikrules.push_back(v49IkRule);
					if (v49IkRule.compressedikerrorindex > 0) errorIdxs.push_back(pos + v49IkRule.compressedikerrorindex);
					if (v49IkRule.compressedikerrorindex > 0) numOfCompressedIkErrors++;
					if (v49IkRule.ikerrorindex > 0) numOfIkErrors++;

					Logger::Info("IkRule Read: %d\n", j);
					numikrules++;
				}
#pragma endregion
#pragma region COMPRESSEDIKERROR
				if (numOfCompressedIkErrors > 0)
				{
					int animStartPos = mdlhdr.localanimindex + 100 * i;
					for (int j = 0; j < numOfCompressedIkErrors; j++)
					{
						Stream->seek(errorIdxs[j]);
						int dist = 0;
						int next = j + 1;
						int pos = Stream->Position();
						mstudiocompressedikerror_t_v49 v49CompressedIkError; Stream->Read(&v49CompressedIkError);
						mstudioanimdata_t_v49 animData;
						int endPos = Stream->Position();
						if (j + 1 > numOfCompressedIkErrors - 1)
						{
							if (animdescs[i].compressedikerrorindex > 0) dist = animDescPos + animdescs[i].compressedikerrorindex - endPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0)  dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - endPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - endPos;
							if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - endPos;
							Stream->Read(&animData, dist);
							v49CompressedIkError.animdata = animData;
							compressedikerrors.push_back(v49CompressedIkError);
							Logger::Info("CompressedIkError Read: %d\n", j);
							break;
						}
						dist = errorIdxs[next] - endPos;
						Stream->Read(&animData, dist);
						v49CompressedIkError.animdata = animData;
						compressedikerrors.push_back(v49CompressedIkError);
					}
				}
#pragma endregion
#pragma region V52COMPRESSEDERROR
				if (animdescs[i].compressedikerrorindex > 0)
				{
					int animStartPos = mdlhdr.localanimindex + 92 * i;
					Stream->seek(animStartPos + animdescs[i].compressedikerrorindex);
					int dist = 0;
					int pos = Stream->Position();
					mstudiocompressedikerror_t_v52 v52CompressedIkError; Stream->Read(&v52CompressedIkError);
					v52compressedikerrors.push_back(v52CompressedIkError);
					mstudioanimdata_t_v49 animData;
					int endPos = Stream->Position();
					if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0)  dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - endPos;
					if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - endPos;
					if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - endPos;
					Stream->Read(&animData, dist);
					Logger::Info("v52CompressedIkError Read: %d\n");
				}

			}

		}
	}
#pragma endregion
#pragma region SEQDESCS
	if (mdlhdr.numlocalseq > 0)
	{
		int actModNum = 0;
		int eventNum = 0;
		Stream->seek(mdlhdr.localseqindex);
		for (int i = 0; i < mdlhdr.numlocalseq; i++)
		{
			int seqPos = mdlhdr.localseqindex + 212 * i;
			Stream->seek(seqPos);
			mstudioseqdesc_t_v52 seqDesc; Stream->Read(&seqDesc);
			seqDesc.szlabel = Stream->ReadNullTermStrTrgt(seqPos + seqDesc.szlabelindex, false);
			seqDesc.szactivityname = Stream->ReadNullTermStrTrgt(seqPos + seqDesc.szactivitynameindex, false);

			seqdescs.push_back(seqDesc);

			Logger::Info("Seq Read: %s : %s : %d \n", seqDesc.szlabel.c_str(), seqDesc.szactivityname.c_str(), i);
		}

		for (int i = 0; i < mdlhdr.numlocalseq; i++)
		{
			int seqPos = mdlhdr.localseqindex + 212 * i;
			int numBlends = seqdescs[i].groupsize[0] * seqdescs[i].groupsize[1];
			int posekeySize = seqdescs[i].groupsize[0] + seqdescs[i].groupsize[1];
			int numAutolayers = seqdescs[i].numautolayers;
			int numActMods = seqdescs[i].numactivitymodifiers;
			int numEvents = seqdescs[i].numevents;
			if (seqdescs[i].weightlistindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].weightlistindex);
				seqweightlist_t_v49 weightlist; Stream->Read(&weightlist, mdlhdr.numbones);
				seqweightlist.push_back(weightlist);
				Logger::Info("Weightlist Read: %d\n", i);
			}

			if (seqdescs[i].animindexindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].animindexindex);
				blendgroup_t_v49 blendgroup; Stream->Read(&blendgroup, numBlends);
				seqdescs[i].blends = blendgroup;
				blends.push_back(blendgroup);
				Logger::Info("BlendGroup Read: %d\n", i);
			}

			if (seqdescs[i].posekeyindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].posekeyindex);
				posekey_t_v49 posekey; Stream->Read(&posekey, posekeySize);
				if (posekeySize == 3)posekey.unk[2] == 0.0;
//				Logger::Info("PosekeyF Read: %f\n", posekey.unk[2]);
				posekeys.push_back(posekey);
				Logger::Info("Posekey Read: %d\n", i);
			}

			if (numAutolayers > 0)
			{
				Stream->seek(seqPos + seqdescs[i].autolayerindex);
				for (int j = 0; j < numAutolayers; j++)
				{
					mstudioautolayer_t_v49 autolayer; Stream->Read(&autolayer);
					autolayers.push_back(autolayer);
					Logger::Info("Autolayer Read: %d\n", j);
				}
			}

			if (numEvents > 0)
			{
				Stream->seek(seqPos + seqdescs[i].eventindex);
				for (int j = 0; j < seqdescs[i].numevents; j++)
				{
					mstudioevent_t_v49 _event; Stream->Read(&_event);
					events.push_back(_event);

					std::string szEventName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].eventindex + 80 * j + _event.szeventindex, false);
					std::string szName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].eventindex + 80 * j + 12, false);

					seqdescs[i].szeventnames.push_back(szEventName);
					seqdescs[i].events.push_back(eventNum);

					Logger::Info("Event Read: %s : %s : %d\n", szName.c_str(), szEventName.c_str(), j);
					eventNum++;
				}
			}

			if (numActMods > 0)
			{
				Stream->seek(seqPos + seqdescs[i].activitymodifierindex);
				for (int j = 0; j < seqdescs[i].numactivitymodifiers; j++)
				{
					mstudioactivitymodifier_t_v53 actmod; Stream->Read(&actmod);

					std::string szName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].activitymodifierindex + 8 * j + actmod.sznameindex, false);

					seqdescs[i].szactivitymodifiernames.push_back(szName);
					actmod.szname = szName;
					activitymodifiers.push_back(actmod);
					seqdescs[i].actmods.push_back(actModNum);
					Logger::Info("ActMod Read: %s : %d\n", seqdescs[i].szactivitymodifiernames[j].c_str(), i);
					actModNum++;
				}
			}
		}
	}
#pragma endregion
#pragma region NODENAMES
	if (mdlhdr.numlocalnodes > 0)
	{
		Stream->seek(mdlhdr.localnodenameindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodename_t_v49 nodeName; Stream->Read(&nodeName);

			std::string szName = Stream->ReadNullTermStrTrgt(nodeName.sznameindex, false);
			nodeName.szname = szName;
			nodenames.push_back(nodeName);
			Logger::Info("NodeName Read: %s : %d\n", nodeName.szname.c_str(), i);
		}
#pragma endregion
#pragma region NODEDATA
		Stream->seek(mdlhdr.localnodeindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodedata_v49 node; Stream->Read(&node, mdlhdr.numlocalnodes * mdlhdr.numlocalnodes);
			nodes.push_back(node);
			Logger::Info("Node Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region BODYPARTS
	if (mdlhdr.numbodyparts > 0)
	{
		Stream->seek(mdlhdr.bodypartindex);
		for (int i = 0; i < mdlhdr.numbodyparts; i++)
		{
			mstudiobodyparts_t_v49 bodyPart; Stream->Read(&bodyPart);
			if (bodyPart.nummodels > 0) nummodels += bodyPart.nummodels;

			std::string szName = Stream->ReadNullTermStrTrgt(mdlhdr.bodypartindex + 16 * i + bodyPart.sznameindex, false);
			bodyPart.szname = szName;

			bodyparts.push_back(bodyPart);
			Logger::Info("Bodypart Read: %s : %d\n", bodyPart.szname.c_str(), i);
		}
	}
#pragma endregion
#pragma region MODELS

	if (nummodels > 0)
	{
		Stream->seek(mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts);
		for (int i = 0; i < nummodels; i++)
		{
			mstudiomodel_t_v49 model; Stream->Read(&model);
			if (model.nummeshes > 0) nummeshes += model.nummeshes;
			models.push_back(model);
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts + 148 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(modelPos, false);

			Logger::Info("Model Read: %s : %d\n", szName.c_str(), i);
		}

	}
#pragma endregion
#pragma region IKCHAINS
	if (mdlhdr.numikchains > 0)
	{
		Stream->seek(mdlhdr.ikchainindex);
		for (int i = 0; i < mdlhdr.numikchains; i++)
		{
			mstudioikchain_t_v49 ikchain; Stream->Read(&ikchain);
			if (ikchain.numlinks > 0) numiklinks += ikchain.numlinks;

			int ikChainPos = mdlhdr.ikchainindex + 16 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(ikChainPos + ikchain.sznameindex, false);
			ikchain.szname = szName;

			ikchains.push_back(ikchain);
			Logger::Info("IkChain Read: %s : %d\n", szName.c_str(), i);
		}
#pragma endregion
#pragma region IKLINKS
		for (int i = 0; i < numiklinks; i++)
		{
			int ikLinkPos = (mdlhdr.ikchainindex + 16 * mdlhdr.numikchains) + 28 * i;
			Stream->seek(ikLinkPos);
			mstudioiklink_t_v49 iklink; Stream->Read(&iklink);
			iklinks.push_back(iklink);
			Logger::Info("IkLink Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region POSEPARAMDESCS
	if (mdlhdr.localposeparamindex > 0)
	{
		Stream->seek(mdlhdr.localposeparamindex);
		for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
		{
			mstudioposeparamdesc_t_v49 poseparamdesc; Stream->Read(&poseparamdesc);
			int poseParamPos = mdlhdr.localposeparamindex + 20 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(poseParamPos + poseparamdesc.sznameindex, false);
			poseparamdesc.szname = szName;

			poseparamdescs.push_back(poseparamdesc);
			Logger::Info("PoseParamDesc Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region ANIMBLOCKS
	if (mdlhdr.numanimblocks > 0)
	{
		Stream->seek(mdlhdr.localnodeindex);
		for (int i = 0; i < mdlhdr.numanimblocks; i++)
		{
			mstudioanimblock_t animBlock; Stream->Read(&animBlock);
			animblocks.push_back(animBlock);
			Logger::Info("AnimBlock Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region MESHES
	if (nummeshes > 0)
	{
		for (int i = 0; i < nummeshes; i++)
		{
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts;
			int meshPos = (modelPos + 148 * nummodels) + (20 * mdlhdr.numlocalposeparameters) + (16 * mdlhdr.numikchains + 28 * numiklinks) + 116 * i;
			Stream->seek(meshPos);
			mstudiomesh_t_v49 mesh; Stream->Read(&mesh);
			meshes.push_back(mesh);

			int texturePos = mdlhdr.textureindex + (64 * (int)mesh.material);
			Stream->seek(texturePos);
			mstudiotexture_t_v53 texture; Stream->Read(&texture);

			std::string szName = Stream->ReadNullTermStrTrgt(texturePos + texture.sznameindex, false);

			Logger::Info("MeshMat Read: %s : %d\n", szName.c_str(), i);
			Logger::Info("Mesh Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region INCLUDEMODEL
	if (mdlhdr.numincludemodels > 0)
	{
		Stream->seek(mdlhdr.includemodelindex);
		for (int i = 0; i < mdlhdr.numincludemodels; i++)
		{
			mstudiomodelgroup_t_v49 includemodel; Stream->Read(&includemodel);
			int includeModelPos = mdlhdr.includemodelindex + 8 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(includeModelPos + includemodel.sznameindex, false);
			includemodel.szname = szName;

			includedmodels.push_back(includemodel);
			Logger::Info("IncludedModel Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region TEXTURES
	if (mdlhdr.numtextures > 0)
	{
		Stream->seek(mdlhdr.textureindex);
		for (int i = 0; i < mdlhdr.numtextures; i++)
		{
			mstudiotexture_t_v49 texture; Stream->Read(&texture);
			int texturePos = mdlhdr.textureindex + 64 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(texturePos + texture.sznameindex, false);
			texture.szname = szName;

			textures.push_back(texture);

			Logger::Info("Texture Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region CDMATERIALS
	if (mdlhdr.numcdtextures > 0)
	{
		Stream->seek(mdlhdr.cdtextureindex);
		for (int i = 0; i < mdlhdr.numcdtextures; i++)
		{
			mstudiotexturedir_t_v49 cdtexture; Stream->Read(&cdtexture);

			std::string szName = Stream->ReadNullTermStrTrgt(cdtexture.sznameindex, false);
			cdtexture.szname = szName;

			cdtextures.push_back(cdtexture);
			Logger::Info("CdTexture Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region SKINFAMILIES
	if (mdlhdr.numskinfamilies > 0)
	{
		if (mdlhdr.numskinref > 0)
		{
			Stream->seek(mdlhdr.skinindex);
			for (int i = 0; i < mdlhdr.numskinfamilies; i++)
			{
				mstudioskingroup_t_v49 skingroup; Stream->Read(&skingroup, mdlhdr.numskinref);
				skingroups.push_back(skingroup);
				Logger::Info("SkinGroup Read: %d\n", i);
			}
		}
	}
#pragma endregion
#pragma region KEYVALUES
	if (mdlhdr.keyvaluesize > 0)
	{
		Stream->seek(mdlhdr.keyvalueindex);
		Stream->Read(&keyvalues, mdlhdr.keyvaluesize);
		Logger::Info("Keyvalues Read: %d\n", mdlhdr.keyvaluesize);
	}
#pragma endregion
#pragma region SRCBONETRANSFORMS
	if (mdlsubhdr.numsrcbonetransform > 0)
	{
		Stream->seek(mdlsubhdr.srcbonetransformindex);
		for (int i = 0; i < mdlsubhdr.numsrcbonetransform; i++)
		{
			mstudiosrcbonetransform_t_v49 srcbonetransform; Stream->Read(&srcbonetransform);

			int srcBonePos = mdlsubhdr.srcbonetransformindex + 100 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(srcBonePos + srcbonetransform.sznameindex, false);

			srcbonetransforms.push_back(srcbonetransform);
			Logger::Info("SrcBoneTransform Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region LINEARBONE
	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlsubhdr.linearboneindex + 408);
		Stream->Read(&linearbone);
		Stream->Read(&linearbonedata, mdlhdr.numbones);
		Logger::Info("LinearBone Read: %d\n", mdlhdr.numbones);
	}
#pragma endregion
#pragma region PERTRIHDR
	if (mdlsubhdr.aabbindex > 0)
	{
		Stream->seek(mdlsubhdr.aabbindex + 408);
		Stream->Read(&pertriheader);
		Logger::Info("AABB Per Triangle Header Read:\n");
	}
#pragma endregion
#pragma region AABBNODES
	if (mdlsubhdr.numaabbnodes > 0)
	{
		for (int i = 0; i < mdlsubhdr.numaabbnodes; i++)
		{
			aabbnode _aabbnode;
			Stream->Read(&_aabbnode);
			aabbnodes.push_back(_aabbnode);
		}
		//		Logger::Info("Pos Read: %d\n", Stream->Position());
		Logger::Info("AABB Nodes Read: %d\n", mdlsubhdr.numaabbnodes);
	}
#pragma endregion
#pragma region AABBLEAVES
	if (mdlsubhdr.numaabbleaves > 0)
	{

		for (int i = 0; i < mdlsubhdr.numaabbleaves; i++)
		{
			aabbleaf _aabbleaf;
			Stream->Read(&_aabbleaf);
			aabbleaves.push_back(_aabbleaf);
		}
		Logger::Info("AABB Leaves Read: %d\n", mdlsubhdr.numaabbleaves);
	}
#pragma endregion
#pragma region AABBVERTS
	if (mdlsubhdr.numaabbverts > 0)
	{
		Stream->Read(&aabbverts, mdlsubhdr.numaabbverts);
//		Logger::Info("Pos Read: %d\n", Stream->Position());
		Logger::Info("AABB Verts Read: %d\n", mdlsubhdr.numaabbverts);
	}
#pragma endregion
#pragma region STRINGTABLE
	Stream->seek(mdlsubhdr.sznameindex + 408);
	Stream->Read(&stringTable, mdlhdr, mdlsubhdr, seqdescs, hitboxsets, attachments, nodenames, bodyparts, ikchains, animdescs, textures, includedmodels, cdtextures, poseparamdescs, srcbonetransforms);
	std::string string(stringTable.mdlname.begin(), stringTable.mdlname.end());
	Logger::Info("mldName: %s\n", string.c_str());
#pragma endregion
	MDL::v52Mdl _v52mdl{mdlhdr,mdlsubhdr,mayabakery,bones,jigglebones,boneflexdrivers,attachments,hitboxsets,hitboxes,bonenametable,animdescs,anims, sectionindexes, sections, ikrules,compressedikerrors,v52compressedikerrors,ikerrors,ikrulezeroframe,seqdescs,blends,posekeys,events,autolayers,activitymodifiers,seqweightlist,nodenames,nodes,bodyparts,models,meshes,ikchains,iklinks,poseparamdescs,animblocks,includedmodels,cdtextures,textures,skingroups,keyvalues,srcbonetransforms,linearbone,linearbonedata, stringTable, pertriheader, aabbnodes, aabbleaves, aabbverts};
	//Logger::Info("test\n");
	return _v52mdl;
}

int MDL::v52Mdl::ConvertFlag(int flag)
{
	int test = 0;
	int flagNum = 0;

	if (flag & STUDIO_ANIM_ANIMROT)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMROT);
		flagNum++;
	}
	if (flag & STUDIO_ANIM_ANIMPOS)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMPOS);
		flagNum++;
	}

	if (flag & STUDIO_ANIM_RAWROT)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT);
		test += STUDIO_ANIM_RAWROT_53;
		flagNum++;
	}
	if (flag & STUDIO_ANIM_RAWROT2)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT2);
		test += STUDIO_ANIM_RAWROT_53;
		flagNum++;
	}
	if (flag & STUDIO_ANIM_RAWPOS)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWPOS);
		test += STUDIO_ANIM_RAWPOS_53;
		flagNum++;
	}

	if (flag & STUDIO_ANIM_DELTA)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_DELTA);
		test += STUDIO_ANIM_DELTA_53;
	}

	if (flagNum == 1)
	{
		if (flag & STUDIO_ANIM_ANIMROT)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMROT);
			test += STUDIO_ANIM_RAWPOS_53;
		}
		if (flag & STUDIO_ANIM_ANIMPOS)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMPOS);
			test += STUDIO_ANIM_RAWROT_53;
		}

		if (flag & STUDIO_ANIM_RAWROT)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT);
			test += STUDIO_ANIM_RAWPOS_53;
		}
		if (flag & STUDIO_ANIM_RAWROT2)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT2);
			test += STUDIO_ANIM_RAWPOS_53;
		}
		if (flag & STUDIO_ANIM_RAWPOS)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWPOS);
			test += STUDIO_ANIM_READBONE_53;
		}
	}

	test += STUDIO_ANIM_RAWSCALE_53;
	if (flag == 0)
	{
		test = 0;
	}
	return test;
}

std::vector<int> MDL::v52Mdl::GetIkChainBones()
{
	std::vector<int> ikChainBones;
	std::vector<int> ikChainBonesChildren;
	std::vector<int> ikChainBonesFinal;

	for (int i = 0; i < iklinks.size(); i++)
	{
		ikChainBones.push_back(iklinks[i].bone);
	}

	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v52 bone = bones[i];
		for (int j = 0; j < ikChainBones.size(); j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, ikChainBones[j]) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBones[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}
	int size = ikChainBonesChildren.size();

	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v52 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}
	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v52 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v52 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v52 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v52 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	for (int i = 0; i < ikChainBonesChildren.size(); i++)
	{
		ikChainBonesFinal.push_back(ikChainBonesChildren[i]);
	}

	for (int i = 0; i < ikChainBones.size(); i++)
	{
		ikChainBonesFinal.push_back(ikChainBones[i]);
	}

	return ikChainBonesFinal;
}

std::vector<int> MDL::v52Mdl::v53GetAnimHdrBytesAdded(bool zeroFirst)
{
	std::vector<int> animBytesAdded;
	int bytesAdded = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int next = i + 1;
		animBytesAdded.push_back(bytesAdded);
		//Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
		if (animdescs[i].sectionindex == 0)
		{
			for (int j = lastHdr; j < anims.size(); j++)
			{
				int headerSize = GetAnimHeaderSize((int)anims[j].flags);
				lastHdr++;
				bytesAdded += (32 - headerSize);
				if (anims[j].nextoffset == 0)
				{

					if (animdescs[i].numikrules > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].ikruleindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim && animdescs[next].sectionindex == 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].sectionindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
			}
		}
		else animBytesAdded[i] = bytesAdded;
	}
	return animBytesAdded;
}

int MDL::v52Mdl::v53GetTotalAnimHdrBytesAdded()
{
	int bytesAdded = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int next = i + 1;

		if (animdescs[i].sectionindex > 0)
		{
			int frames = animdescs[i].numframes;
			int secFrames = animdescs[i].sectionframes;
			int num = (frames / secFrames) + 2;

			bytesAdded += -4 * num;

		}

		if (animdescs[i].sectionindex == 0)
		{
			for (int j = lastHdr; j < anims.size(); j++)
			{
				int headerSize = GetAnimHeaderSize((int)anims[j].flags);
				lastHdr++;
				bytesAdded += (32 - headerSize);
				if (anims[j].nextoffset == 0)
				{

					if (animdescs[i].numikrules > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].ikruleindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						bytesAdded += 32 - dist;
					}

					if (i + 1 > mdlhdr.numlocalanim - 1)
					{
						int dist = (mdlhdr.localseqindex - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("secDist:  %d\n", dist);
						//Logger::Info("secFinalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim && animdescs[next].sectionindex == 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].sectionindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
			}
		}
	}
	//Logger::Info("Total BytesAdded: %d\n", bytesAdded);
	return bytesAdded;
}

int MDL::v52Mdl::v53GetAnimHdrBytesAddedIdv(int anim)
{
	int bytesAdded = 0;
	int lastHdr = 0;

	for (int i = 0; i < anim + 1; i++)
	{
		int next = i + 1;
		if (animdescs[i].sectionindex == 0)
		{
			bytesAdded = 0;
			for (int j = lastHdr; j < anims.size(); j++)
			{
				int headerSize = GetAnimHeaderSize((int)anims[j].flags);
				lastHdr++;
				bytesAdded += (32 - headerSize);
				if (anims[j].nextoffset == 0)
				{

					if (animdescs[i].numikrules > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].ikruleindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						bytesAdded += 32 - dist;
					}

					if (i + 1 > mdlhdr.numlocalanim - 1)
					{
						int dist = (mdlhdr.localseqindex - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("secDist:  %d\n", dist);
						//Logger::Info("secFinalDist:  %d\n", 32 - dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex == 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].sectionindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
			}
		}
	}
	return bytesAdded;
}

std::vector<int> MDL::v52Mdl::v53GetSecHdrBytesAdded(bool zeroFirst)
{
	int bytesAdded = 0;
	std::vector<int> bytesAddedPerAnim;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		bytesAddedPerAnim.push_back(bytesAdded);
		//Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			int secDist = startPos + animdescs[i].animindex - (startPos + animdescs[i].sectionindex + (8 * num));

			bytesAdded -= secDist;

			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{

						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 > mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = (mdlhdr.localseqindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));
							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}
						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 < num - 1)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));
							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}
						break;
					}
				}
				secNum++;
			}
		}
	}
	return bytesAddedPerAnim;
}

int MDL::v52Mdl::v53GetTotalSecHdrBytesAdded()
{
	int bytesAdded = 0;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			int secDist = startPos + animdescs[i].animindex - (startPos + animdescs[i].sectionindex + (8 * num));

			bytesAdded -= secDist;

			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}

						if (i + 1 > mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = (mdlhdr.localseqindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}

						if (j + 1 < num - 1)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}
						break;
					}
				}
				secNum++;
			}
		}
	}
	//Logger::Info("Total BytesAdded: %d\n", bytesAdded);
	return bytesAdded;
}

int MDL::v52Mdl::v53GetSecHdrBytesAddedIdv(int anim)
{
	int bytesAdded = 0;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < anim + 1; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		//bytesAddedPerAnim.push_back(bytesAdded);
		//Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			int secDist = startPos + animdescs[i].animindex - (startPos + animdescs[i].sectionindex + (8 * num));

			bytesAdded -= secDist;
			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{

						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 > mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = (mdlhdr.localseqindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (j + 1 < num - 1)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}
						break;
					}
				}
				secNum++;
			}
		}
	}
	return bytesAdded;
}

std::vector<int> MDL::v52Mdl::v53GetSecBytesAdded(bool zeroFirst)
{
	int bytesAdded = 0;
	std::vector<int> bytesAddedPerSec;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			int secDist = startPos + animdescs[i].animindex - (startPos + animdescs[i].sectionindex + (8 * num));

			bytesAdded -= secDist;

			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				bytesAddedPerSec.push_back(bytesAdded);
				//Logger::Info("secBytesAdded: %d, Anim: %d\n", bytesAdded, i);
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (j + 1 < num - 1)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						break;
					}
				}
				secNum++;
			}
		}
	}
	return bytesAddedPerSec;
}

std::vector<int> MDL::v52Mdl::v53IkRuleStairsPerAnim()
{
	std::vector<int> ikRuleStairsPerAnim;
	int ikRuleNum = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		mstudioanimdesc_t_v52 v52AnimDesc = animdescs[i];
		int ikStairs = -12 * (ikRuleNum);

		ikRuleStairsPerAnim.push_back(ikStairs);
		if (v52AnimDesc.numikrules > 0)
		{
			ikRuleNum += v52AnimDesc.numikrules;
		}

	}
	return ikRuleStairsPerAnim;
}

void MDL::v52Mdl::SetMdlInts()
{
	ikRuleStairsPerAnim = v53IkRuleStairsPerAnim();
	hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	secHdrBytesSecAdd = v53GetSecBytesAdded(true);

	animByteAddedTotal = v53GetTotalAnimHdrBytesAdded();
	animSecByteAddedTotal = v53GetTotalSecHdrBytesAdded();

	animBytesAdded = animByteAddedTotal + animSecByteAddedTotal;
	bytesAddedToRuiMesh = 0;
	bytesAddedToIkRules = -12 * ikrules.size();
	bytesAddedToHeader = 52;
	bytesAddedToBones = mdlhdr.numbones * 28;
	bytesAddedToAnims = -8 * mdlhdr.numlocalanim;
	bytesAddedToAnimData = mdlhdr.numlocalanim > 0 ? animByteAddedTotal + animSecByteAddedTotal + bytesAddedToIkRules + bytesAddedToSections : 0;
	bytesAddedToSections = -4 * secHdrBytesSecAdd.size();
	bytesAddedToSeqs = 20 * mdlhdr.numlocalseq;
	bytesAddedToTextures = -20 * mdlhdr.numtextures;
	bytesAddedToIkChains = 16 * mdlhdr.numikchains;
	bytesAddedToActMods = 0;
	bytesAddedToLinearBone = -12 * mdlhdr.numbones;
	textureFiller = 0;
	strFiller = 0;
	allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh;
	numOfLinks = 0;
}

studiohdr_t_v53 MDL::v52Mdl::ConvertHeader(FileInfo info)
{

	int ruiNum = 0;
	int mdlSize = 0;
	int phySize = 0;
	int phyBSize = 0;
	int vtxSize = 0;
	int vvdSize = 0;
	int vvcSize = 0;
	int ruiSize = 0;

	if (info.rui.has_value()) //This is a temp for rui testing. -Liberty
	{
		BinaryReader RUIStream = BinaryReader(info.rui.value().c_str());
		bytesAddedToRuiMesh = RUIStream.size;
		ruiSize = RUIStream.size;
		bytesAddedToRuiMesh = ruiSize;
		mstudiorruiheader_t ruiHeader;
		mstudioruimesh_t ruiMesh;
		int meshIndex = 0;
		RUIStream.Read(&ruiHeader);
		RUIStream.seek(ruiHeader.ruimeshindex);
		//		Logger::Info("Index:  %d\n", RUIStream.size);

		for (int i = 0; i < 64; i++)
		{
			//RUIStream.Read(&ruiHeader);
			//if (i == 0)meshIndex = ruiHeader.ruimeshindex;
			//if (i == 0)RUIStream.seek(ruiHeader.ruimeshindex);
			RUIStream.Read(&ruiMesh);
			ruiNum++;
			//			Logger::Info("Pos:  %d Num: %d\n", RUIStream.Position(), i);


			if (RUIStream.Position() >= RUIStream.size)
			{
				break;
			}
		}
		RUIStream.seek(0);
		RUIStream.Read(&ruiHeader);
		int ruiPos = mdlhdr.includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + 8 * mdlhdr.numincludemodels;
		int dist = ruiHeader.ruimeshindex - 8 * ruiNum;
		//		Logger::Info("Index:  %d\n", ruiHeader.ruimeshindex);
		double ruiAlignment = (ruiPos + 8 * ruiNum) / (double)16;
		//		Logger::Info("Alignment:  %f\n", ruiAlignment);
		if (!IsWhole(ruiAlignment))
		{
			int ruiAlignment2 = 16 * ((int)ruiAlignment + 1);
			int paddingFix = ruiAlignment2 - ruiPos;
			//			Logger::Info("Padding:  %d\n", paddingFix - 8 * ruiNum);
			//			Logger::Info("Padding2:  %d\n", ruiAlignment2);
			//			Logger::Info("Padding3:  %d\n", ruiPos);
			ruiPadding = paddingFix - 8 * ruiNum;
			ruiSize += paddingFix - 8 * ruiNum;
			bytesAddedToRuiMesh = ruiSize;
		}
	}


	if (info.mdl.has_value())
	{
		BinaryReader MdlStream = BinaryReader(info.mdl.value().c_str());
		mdlSize = MdlStream.size;
	}
	if (info.phy.has_value())
	{
		BinaryReader PhyStream = BinaryReader(info.phy.value().c_str());
		phySize = PhyStream.size;
	}
	if (info.pfb.has_value())
	{
		BinaryReader PhyBStream = BinaryReader(info.pfb.value().c_str());
		phyBSize = PhyBStream.size;
	}
	if (info.vtx.has_value())
	{
		BinaryReader VtxStream = BinaryReader(info.vtx.value().c_str());
		vtxSize = VtxStream.size;
	}
	if (info.vvd.has_value())
	{
		BinaryReader VvdStream = BinaryReader(info.vvd.value().c_str());
		vvdSize = VvdStream.size;
	}

	if (info.vvc.has_value())
	{
		BinaryReader VvcStream = BinaryReader(info.vvc.value().c_str());
		vvcSize = VvcStream.size;
	}

	//Logger::Notice("RuiSize %d\n", ruiSize);

	int			id;
	int			version;
	int			checksum;
	int			sznameindex = mdlsubhdr.sznameindex + allBytesAdded + 408 + ruiSize + bytesAddedToLinearBone;
	char		name[64];

	int			length;

	Vector3		eyeposition;

	Vector3		illumposition;

	Vector3		hull_min;
	Vector3		hull_max;

	Vector3		view_bbmin;
	Vector3		view_bbmax;

	mdlflags_t	flags;

	int			numbones;
	int			boneindex = mdlhdr.boneindex + bytesAddedToHeader;

	int			numbonecontrollers;
	int			bonecontrollerindex = mdlhdr.bonecontrollerindex + bytesAddedToHeader + bytesAddedToBones;

	int			numhitboxsets;
	int			hitboxsetindex = mdlhdr.hitboxsetindex + bytesAddedToHeader + bytesAddedToBones;

	int			numlocalanim;
	int			localanimindex = mdlhdr.localanimindex + bytesAddedToHeader + bytesAddedToBones;

	int			numlocalseq;
	int			localseqindex = mdlhdr.localseqindex + bytesAddedToAnims + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;

	int			activitylistversion;
	int			eventsindexed;

	int			numtextures;
	int			textureindex = mdlhdr.textureindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + ruiSize;

	int			numcdtextures;
	int			cdtextureindex = mdlhdr.cdtextureindex + allBytesAdded + ruiSize;

	int			numskinref;
	int			numskinfamilies;
	int			skinindex = mdlhdr.skinindex + allBytesAdded + ruiSize;

	int			numbodyparts;
	int			bodypartindex = mdlhdr.bodypartindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

	int			numlocalattachments;
	int			localattachmentindex = mdlhdr.localattachmentindex + bytesAddedToHeader + bytesAddedToBones;

	int			numlocalnodes;
	int			localnodeindex = mdlhdr.localnodeindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;
	int			localnodenameindex = mdlhdr.localnodenameindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

	int			numflexdesc;
	int			flexdescindex = mdlhdr.flexdescindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			numflexcontrollers;
	int			flexcontrollerindex = mdlhdr.flexcontrollerindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			numflexrules;
	int			flexruleindex = mdlhdr.flexruleindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			numikchains;
	int			ikchainindex = mdlhdr.ikchainindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

	int			nummouths = ruiNum;
	int			mouthindex = mdlhdr.includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + 8 * mdlhdr.numincludemodels;

	int			numlocalposeparameters;
	int			localposeparamindex = mdlhdr.localposeparamindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			surfacepropindex = mdlhdr.surfacepropindex + allBytesAdded + ruiSize + bytesAddedToLinearBone;

	int			keyvalueindex = mdlhdr.keyvalueindex + allBytesAdded + ruiSize;
	int			keyvaluesize;

	int			numlocalikautoplaylocks;
	int			localikautoplaylockindex = mdlhdr.localikautoplaylockindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;


	float		mass;
	int			contents;

	int			numincludemodels;
	int			includemodelindex = mdlhdr.includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	uint32_t	virtualModel;

	int			bonetablebynameindex = mdlhdr.bonetablebynameindex + bytesAddedToHeader + bytesAddedToBones;

	std::byte	constdirectionallightdot;

	std::byte	rootLOD;

	std::byte	numAllowedRootLODs;

	std::byte	unused;

	float		fadedistance;

	int			numflexcontrollerui;
	int			flexcontrolleruiindex = mdlhdr.flexcontrolleruiindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	uint32_t	pVertexBase;
	uint32_t	pIndexBase;

	int			mayaindex = mdlhdr.mayaindex + bytesAddedToHeader;

	int			numsrcbonetransform;
	int			srcbonetransformindex = mdlsubhdr.srcbonetransformindex + allBytesAdded + ruiSize;

	int			illumpositionattachmentindex;

	int			linearboneindex = mdlsubhdr.linearboneindex + allBytesAdded + 408 + ruiSize;

	int			m_nBoneFlexDriverCount;
	int			m_nBoneFlexDriverIndex;

	int			aabbindex = mdlsubhdr.aabbindex + allBytesAdded + 408 + ruiSize + bytesAddedToLinearBone;
	int			numaabbnodes = mdlsubhdr.numaabbnodes;
	int			numaabbleaves = mdlsubhdr.numaabbleaves;
	int			numaabbverts = mdlsubhdr.numaabbverts;

	int			unkstringindex = mdlsubhdr.unkstringindex + allBytesAdded + 408 + ruiSize + bytesAddedToLinearBone;

	int			vtxindex = vtxSize > 0 ? mdlSize + allBytesAdded + phySize + phyBSize + ruiSize + bytesAddedToLinearBone : 0; // VTX
	int			vvdindex = vvdSize > 0 ? mdlSize + allBytesAdded + phySize + vtxSize + phyBSize + ruiSize + bytesAddedToLinearBone : 0; // VVD / IDSV
	int			vvcindex = vvcSize > 0 ? mdlSize + allBytesAdded + phySize + vtxSize + phyBSize + ruiSize + vvdSize + bytesAddedToLinearBone : 0; // VVC / IDCV 
	int			vphyindex = phySize > 0 ? mdlSize + allBytesAdded + ruiSize + bytesAddedToLinearBone : 0; // VPHY / IVPS

	int			vtxsize = vtxSize; // VTX
	int			vvdsize = vvdSize; // VVD / IDSV
	int			vvcsize = vvcSize; // VVC / IDCV 
	int			vphysize = phySize; // VPHY / IVPS

	int			unkmemberindex1 = mdlSize + allBytesAdded + phySize + ruiSize + bytesAddedToLinearBone;
	int			numunkmember1 = phyBSize > 0 ? 0 : 0;

	int			unk = 0;

	int			unkindex3 = mdlSize + allBytesAdded + phySize + ruiSize + bytesAddedToLinearBone;

	int			unused1[60];

	studiohdr_t_v53 v53Hdr = { mdlhdr.id, 53, mdlhdr.checksum, sznameindex, ArrayToVector(mdlhdr.name, 64), mdlhdr.length, mdlhdr.eyeposition, mdlhdr.illumposition, mdlhdr.hull_min, mdlhdr.hull_max, mdlhdr.view_bbmin, mdlhdr.view_bbmax, mdlhdr.flags, mdlhdr.numbones, boneindex, mdlhdr.numbonecontrollers, bonecontrollerindex, mdlhdr.numhitboxsets, hitboxsetindex, mdlhdr.numlocalanim, localanimindex, mdlhdr.numlocalseq, localseqindex, mdlhdr.activitylistversion, mdlhdr.eventsindexed, mdlhdr.numtextures, textureindex, mdlhdr.numcdtextures, cdtextureindex, mdlhdr.numskinref, mdlhdr.numskinfamilies, skinindex, mdlhdr.numbodyparts, bodypartindex, mdlhdr.numlocalattachments, localattachmentindex, mdlhdr.numlocalnodes, localnodeindex, localnodenameindex, mdlhdr.numflexdesc, flexdescindex, mdlhdr.numflexcontrollers, flexcontrollerindex, mdlhdr.numflexrules, flexruleindex, mdlhdr.numikchains, ikchainindex, ruiNum, mouthindex, mdlhdr.numlocalposeparameters, localposeparamindex, surfacepropindex, keyvalueindex, mdlhdr.keyvaluesize, mdlhdr.numlocalikautoplaylocks, localikautoplaylockindex, mdlhdr.mass, mdlhdr.contents, mdlhdr.numincludemodels, includemodelindex, mdlhdr.virtualModel, bonetablebynameindex, mdlhdr.constdirectionallightdot, mdlhdr.rootLOD, mdlhdr.numAllowedRootLODs, mdlhdr.unused, mdlhdr.fadedistance, mdlhdr.numflexcontrollerui, flexcontrolleruiindex, mdlhdr.pVertexBase, mdlhdr.pIndexBase, mayaindex, mdlsubhdr.numsrcbonetransform, srcbonetransformindex, mdlsubhdr.illumpositionattachmentindex, linearboneindex, mdlsubhdr.m_nBoneFlexDriverCount, mdlsubhdr.m_nBoneFlexDriverIndex, aabbindex, numaabbnodes, numaabbleaves, numaabbverts, unkstringindex, vtxindex, vvdindex, vvcindex, vphyindex, vtxsize, vvdsize, vvcsize, vphysize, unkmemberindex1, numunkmember1, unk, unkindex3, unused1 };

	return v53Hdr;
}

std::vector<mstudiobone_t_v53> MDL::v52Mdl::BoneConversion()
{
	std::vector<mstudiobone_t_v53> v53Bones;
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		int stairs = bytesAddedToAnims + bytesAddedToSeqs + ( 28 * (mdlhdr.numbones - i) ) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone;
		int steps = (28 * (mdlhdr.numbones - i));

		mstudiobone_t_v52 v52Bone = bones[i];

		Vector3 posscale{ 0,0,0 };

		if (v52Bone.procindex > 0) v52Bone.procindex += steps;
		if (v52Bone.surfacepropidx > 0) v52Bone.surfacepropidx += stairs;
		if (v52Bone.surfacepropLookup > 0) v52Bone.surfacepropLookup += stairs;

		mstudiobone_t_v53 bone{ v52Bone.sznameindex + stairs, v52Bone.parent, -1,-1,-1,-1,-1,-1, v52Bone.pos, v52Bone.quat, v52Bone.rot, v52Bone.unkvector, posscale, v52Bone.rotscale, v52Bone.unkvector1, v52Bone.poseToBone, v52Bone.qAlignment, v52Bone.flags, v52Bone.proctype, v52Bone.procindex, v52Bone.physicsbone, v52Bone.surfacepropidx, v52Bone.contents, v52Bone.surfacepropLookup, -1, 0, 0, 0, 0, 0, 0, 0, 0 };

		v53Bones.push_back(bone);
	}
	return v53Bones;
}

std::vector<mstudiobbox_t_v53> MDL::v52Mdl::HitboxConversion()
{
	std::vector<mstudiobbox_t_v53> v53Hitboxes;
	int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
	for (int i = 0; i < hitboxes.size(); i++)
	{
		mstudiobbox_t_v49 v49Hitbox = hitboxes[i];

		mstudiobbox_t_v53 hitbox{ v49Hitbox.bone, v49Hitbox.group, v49Hitbox.bbmin, v49Hitbox.bbmax, v49Hitbox.szhitboxnameindex + stairs + bytesAddedToLinearBone, 0, v49Hitbox.szhitboxnameindex + stairs + bytesAddedToLinearBone, 0,0,0,0,0,0 };

		v53Hitboxes.push_back(hitbox);
	}

	return v53Hitboxes;
}

std::vector<mstudioanimdesc_t_v53> MDL::v52Mdl::AnimDescConversion()
{
	std::vector<int> hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	std::vector<int> secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	std::vector<mstudioanimdesc_t_v53> v53AnimDescs;
	int ikRuleNum = 0;
	int animIdx = mdlhdr.localanimindex;
	//Logger::Info("animIdx: %d\n", animIdx);
	//Logger::Info("Hdr BytesAdded: %d\n", bytesAddedToHeader);
	//Logger::Info("Bone BytesAdded: %d\n", bytesAddedToBones);
	int bytesAddedToHeader = 52;
	int bytesAddedToBones = mdlhdr.numbones * 28;
	int secNum = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		//Logger::Info("Test\n");
		mstudioanimdesc_t_v52 v49AnimDesc = animdescs[i];

		int frames = v49AnimDesc.numframes;
		int secFrames = v49AnimDesc.sectionframes;
		int outPos = animIdx + bytesAddedToHeader + bytesAddedToBones + 92 * i;
		int pos = animIdx + 100 * i;
		int stairs = (-((8 * (mdlhdr.numlocalanim - i))) + bytesAddedToSeqs) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
		int steps = -((8 * (mdlhdr.numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i];

		int ikStairs = -12 * (ikRuleNum);
		if (v49AnimDesc.numikrules > 0)
		{
			int stairs = v53GetAnimHdrBytesAddedIdv(i);
			int stairs2 = 0;
			if (v49AnimDesc.sectionindex > 0) stairs2 = v53GetSecHdrBytesAddedIdv(i);
			v49AnimDesc.ikruleindex += stairs + stairs2;
			ikRuleNum += v49AnimDesc.numikrules;
		}
		int secStairs = -4 * secNum;
		if (v49AnimDesc.sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;
			secNum += num;
			int dist = (((pos + animdescs[i].sectionindex) + (8 * num)) - (pos + animdescs[i].animindex)) * -1;
		}

		if (v49AnimDesc.animindex > 0) v49AnimDesc.animindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.movementindex > 0) v49AnimDesc.movementindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.ikruleindex > 0) v49AnimDesc.ikruleindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.localhierarchyindex > 0) v49AnimDesc.localhierarchyindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.zeroframeindex > 0) v49AnimDesc.zeroframeindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.ikrulezeroframeindex > 0) v49AnimDesc.ikrulezeroframeindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.sectionindex > 0) v49AnimDesc.sectionindex += steps + ikStairs + secStairs;

		int compressedIkRuleIdx = 0; //if (v49AnimDesc.numikrules > 0) compressedIkRuleIdx = ( basePtr - ( basePtr - (v49AnimDesc.ikruleindex + 140 * v49AnimDesc.numikrules) ) ) * -1 ;


		mstudioanimdesc_t_v53 animDesc = { -outPos, v49AnimDesc.sznameindex + stairs + bytesAddedToLinearBone, v49AnimDesc.fps, v49AnimDesc.flags, v49AnimDesc.numframes, v49AnimDesc.nummovements, v49AnimDesc.movementindex, compressedIkRuleIdx * -1, v49AnimDesc.animindex, v49AnimDesc.numikrules, v49AnimDesc.ikruleindex, v49AnimDesc.numlocalhierarchy, v49AnimDesc.localhierarchyindex, v49AnimDesc.sectionindex, v49AnimDesc.sectionframes, v49AnimDesc.zeroframespan, v49AnimDesc.zeroframecount, v49AnimDesc.zeroframeindex, v49AnimDesc.zeroframestalltime, 0, 0, 0, 0, 0 };

		v53AnimDescs.push_back(animDesc);
	}
	return v53AnimDescs;
}

std::vector<mstudioseqdesc_t_v53> MDL::v52Mdl::SeqDescConversion()
{
	std::vector<int> hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	std::vector<int> secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	std::vector<mstudioseqdesc_t_v53> v53SeqDescs;
	int ikRuleNum = 0;
	int seqIdx = mdlhdr.localseqindex;
	//Logger::Info("animIdx: %d\n", seqIdx);
	//Logger::Info("Hdr BytesAdded: %d\n", bytesAddedToHeader);
	//Logger::Info("Bone BytesAdded: %d\n", bytesAddedToBones);
	int bytesAddedToHeader = 52;
	int bytesAddedToBones = mdlhdr.numbones * 28;

	for (int i = 0; i < mdlhdr.numlocalseq; i++)
	{
		//Logger::Info("Test\n");
		mstudioseqdesc_t_v52 v52SeqDesc = seqdescs[i];
		int strPos = mdlhdr.localseqindex + 212 * i;

		int steps = 20 * (mdlhdr.numlocalseq - i);
		int stairs = steps + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh;

		int outPos = seqIdx + stairs;

		if (v52SeqDesc.eventindex > 0)				v52SeqDesc.eventindex += steps;
		if (v52SeqDesc.animindexindex > 0)			v52SeqDesc.animindexindex += steps;
		if (v52SeqDesc.movementindex > 0)			v52SeqDesc.movementindex += steps;
		if (v52SeqDesc.autolayerindex > 0)			v52SeqDesc.autolayerindex += steps;
		if (v52SeqDesc.weightlistindex > 0)			v52SeqDesc.weightlistindex += steps;
		if (v52SeqDesc.posekeyindex > 0)			v52SeqDesc.posekeyindex += steps;
		if (v52SeqDesc.iklockindex > 0)				v52SeqDesc.iklockindex += steps;
		if (v52SeqDesc.keyvalueindex > 0)			v52SeqDesc.keyvalueindex += steps;
		if (v52SeqDesc.cycleposeindex > 0)			v52SeqDesc.cycleposeindex += steps;
		if (v52SeqDesc.activitymodifierindex > 0)	v52SeqDesc.activitymodifierindex += steps;

		mstudioseqdesc_t_v53 v53SeqDesc{ -outPos, v52SeqDesc.szlabelindex + stairs + bytesAddedToLinearBone, v52SeqDesc.szactivitynameindex + stairs + bytesAddedToLinearBone, v52SeqDesc.flags, v52SeqDesc.activity, v52SeqDesc.actweight, v52SeqDesc.numevents, v52SeqDesc.eventindex, v52SeqDesc.bbmin, v52SeqDesc.bbmax, v52SeqDesc.numblends, v52SeqDesc.animindexindex, v52SeqDesc.movementindex, v52SeqDesc.groupsize[0], v52SeqDesc.groupsize[1], v52SeqDesc.paramindex[0], v52SeqDesc.paramindex[1], v52SeqDesc.paramstart[0], v52SeqDesc.paramstart[1], v52SeqDesc.paramend[0] , v52SeqDesc.paramend[1], v52SeqDesc.paramparent, v52SeqDesc.fadeintime, v52SeqDesc.fadeouttime, v52SeqDesc.localentrynode, v52SeqDesc.localexitnode, v52SeqDesc.nodeflags, v52SeqDesc.entryphase, v52SeqDesc.exitphase, v52SeqDesc.lastframe, v52SeqDesc.nextseq, v52SeqDesc.pose, v52SeqDesc.numikrules, v52SeqDesc.numautolayers, v52SeqDesc.autolayerindex, v52SeqDesc.weightlistindex, v52SeqDesc.posekeyindex, v52SeqDesc.numiklocks, v52SeqDesc.iklockindex, v52SeqDesc.keyvalueindex, v52SeqDesc.keyvaluesize, v52SeqDesc.cycleposeindex, v52SeqDesc.activitymodifierindex, v52SeqDesc.numactivitymodifiers, v52SeqDesc.unk, v52SeqDesc.unk1,0,0,0,0,0,0,0,0 };

		v53SeqDescs.push_back(v53SeqDesc);
	}
	return v53SeqDescs;
}

std::vector<mstudioikrule_t_v53> MDL::v52Mdl::IkRuleConversion()
{
	std::vector<mstudioikrule_t_v53> v53IkRules;
	int numIkRules = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		mstudioanimdesc_t_v52 v52AnimDesc = animdescs[i];
		if (v52AnimDesc.numikrules > 0)
		{
			for (int j = 0; j < v52AnimDesc.numikrules; j++)
			{
				int ikStairs = -12 * (v52AnimDesc.numikrules - j);

				mstudioikrule_t_v49 v49IkRule = ikrules[numIkRules];
				numIkRules++;

				if (v49IkRule.compressedikerrorindex > 0) v49IkRule.compressedikerrorindex += ikStairs;
				if (v49IkRule.ikerrorindex > 0) v49IkRule.ikerrorindex += ikStairs;
				if (v49IkRule.szattachmentindex > 0) v49IkRule.szattachmentindex += ikStairs;

				mstudioikrule_t_v53 ikRule = { v49IkRule.index, ikruletype(v49IkRule.type), v49IkRule.chain, v49IkRule.bone, v49IkRule.slot, v49IkRule.height, v49IkRule.radius, v49IkRule.floor, v49IkRule.pos, v49IkRule.q.one, v49IkRule.q.i, v49IkRule.q.j, v49IkRule.q.k, v49IkRule.compressedikerrorindex, v49IkRule.iStart, v49IkRule.ikerrorindex, v49IkRule.start, v49IkRule.peak, v49IkRule.tail, v49IkRule.end, v49IkRule.contact, v49IkRule.drop, v49IkRule.top, v49IkRule.szattachmentindex, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

				v53IkRules.push_back(ikRule);
			}
		}
	}
	return v53IkRules;
}

std::vector<mstudioanim_t_v53> MDL::v52Mdl::ConvertAnims()
{
	std::vector<mstudioanim_t_v49> _anims = anims;
	std::vector<mstudioanim_t_v53> v53Anims;
	mstudiolinearbonedata_t_v49 lbd = linearbonedata;

	for (int i = 0; i < anims.size(); i++)
	{

		bool hasAnimPosPtr = (int)anims[i].flags & STUDIO_ANIM_ANIMPOS;
		bool hasAnimRotPtr = (int)anims[i].flags & STUDIO_ANIM_ANIMROT;
		bool hasRawPos = (int)anims[i].flags & STUDIO_ANIM_RAWPOS;
		bool hasRawRot = (int)anims[i].flags & STUDIO_ANIM_RAWROT;
		bool hasRawRot2 = (int)anims[i].flags & STUDIO_ANIM_RAWROT2;


		float _posscale = 0; // does what posscale is used for

		std::byte _bone; // unsigned byte, bone limit exceeds 128 so has to be. also means max bones is 255.
		std::byte _flags;

		int16_t _unk = 0; // normally null data


		QuaternionShort _rawrot = { 0,0,0, 0 }; // pQuat64

		mstudioanim_valueptr_t_v49 _animrot = { 0,0,0 }; // pRotV
		int16_t _unused = 0; // pretty sure


		Vector3Short _rawpos = { 0,0,0 }; // pPos
		mstudioanim_valueptr_t_v49 _animpos = { 0,0,0 }; // pPosV

		Vector3Short _rawscale = { 15360, 15360, 15360 }; // new in v53
		mstudioanim_valueptr_t_v49 _animscale = { 0,0,0 };

		int _nextOffset = (int)anims[i].nextoffset > 0 ? (int)anims[i].nextoffset + (32 - GetAnimHeaderSize((int)anims[i].flags)) : 0;

		int bone = (int)anims[i].bone;
		_bone = anims[i].bone;
		_flags = (std::byte)ConvertFlag((int)anims[i].flags);

		if (hasAnimPosPtr && hasAnimRotPtr) _posscale = GetLargestNumber(lbd.posScale[bone].x, lbd.posScale[bone].y, lbd.posScale[bone].z);
		if ((int)anims[i].flags & STUDIO_ANIM_ANIMROT)
		{
			Vector3Short rotPtr = anims[i].animrot.offset;
			if (!hasAnimPosPtr)
			{
				rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
				anims[i].animrot.offset = rotPtr;
			}
			else
			{
				rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
				anims[i].animrot.offset = rotPtr;
			}
			_animrot = anims[i].animrot;

		}

		if ((int)anims[i].flags & STUDIO_ANIM_ANIMPOS)
		{
			Vector3Short posPtr = anims[i].animpos.offset;
			if (!hasAnimRotPtr)
			{
				posPtr.x > 0 ? posPtr.x += 0 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 0 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 0 : posPtr.z = 0;
				anims[i].animpos.offset = posPtr;
			}
			else
			{
				posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
				anims[i].animpos.offset = posPtr;
			}
			_animpos = anims[i].animpos;
		}

		if ((int)anims[i].flags & STUDIO_ANIM_RAWROT) _rawrot = anims[i].rawrot;

		if ((int)anims[i].flags & STUDIO_ANIM_RAWROT2) _rawrot = anims[i].rawrot2;

		if ((int)anims[i].flags & STUDIO_ANIM_RAWPOS) _rawpos = anims[i].rawpos;
		else
			_rawpos = { (short)float_to_half(lbd.bonePos[bone].x), (short)float_to_half(lbd.bonePos[bone].y), (short)float_to_half(lbd.bonePos[bone].z) };

		mstudioanim_t_v53 v53Anim{ _posscale, _bone, _flags, _unk, _rawrot, _animrot, _unused, _rawpos, _animpos, _rawscale, _animscale, _nextOffset, anims[i].animdata };
		v53Anims.push_back(v53Anim);
	}
	return v53Anims;
}

std::vector<sectionindexes_t_v53> MDL::v52Mdl::ConvertSectionIndexes()
{
	std::vector<sectionindexesindex_t_v49> _sectionIndexes = sectionindexes;
	std::vector<sectionindexes_t_v53> v53SectionIndexes;
	int secNum = 0;
	int secSetNum = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int pos = mdlhdr.localanimindex + 100 * i;
		int stairs = -((8 * (mdlhdr.numlocalanim - i))) + hdrBytesAnimDescAdd[i];

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;
			secSetNum += num;

			int dist = pos + animdescs[i].animindex - (pos + animdescs[i].sectionindex + (8 * num));
			//Logger::Info("DistStr: %d\n", pos + animdescs[i].sectionindex + (8 * num) );
			//Logger::Info("DistEnd: %d\n", pos + animdescs[i].animindex);
			//Logger::Info("Dist: %d\n", dist);

			for (int j = 0; j < num; j++)
			{
				int steps = -4 * secSetNum;
				int offset = sectionindexes[secNum].sectionoffset + stairs + secHdrBytesSecAdd[secNum] + steps;

				sectionindexes_t_v53 v53SectionIndex{ offset };
				v53SectionIndexes.push_back(v53SectionIndex);
				secNum++;
			}
		}
	}
	return v53SectionIndexes;
}

std::vector<mstudioanim_t_v53> MDL::v52Mdl::ConvertSections()
{
	std::vector<mstudioanim_t_v49> _sections = sections;
	std::vector<mstudioanim_t_v53> v53Sections;
	mstudiolinearbonedata_t_v49 lbd = linearbonedata;

	for (int i = 0; i < sections.size(); i++)
	{


		bool hasAnimPosPtr = (int)sections[i].flags & STUDIO_ANIM_ANIMPOS;
		bool hasAnimRotPtr = (int)sections[i].flags & STUDIO_ANIM_ANIMROT;
		bool hasRawPos = (int)sections[i].flags & STUDIO_ANIM_RAWPOS;
		bool hasRawRot = (int)sections[i].flags & STUDIO_ANIM_RAWROT;
		bool hasRawRot2 = (int)sections[i].flags & STUDIO_ANIM_RAWROT2;

		float _posscale = 0; // does what posscale is used for

		std::byte _bone; // unsigned byte, bone limit exceeds 128 so has to be. also means max bones is 255.
		std::byte _flags;

		int16_t _unk = 0; // normally null data


		QuaternionShort _rawrot = { 0,0,0, 0 }; // pQuat64

		mstudioanim_valueptr_t_v49 _animrot = { 0,0,0 }; // pRotV
		int16_t _unused = 0; // pretty sure


		Vector3Short _rawpos = { 0,0,0 }; // pPos
		mstudioanim_valueptr_t_v49 _animpos = { 0,0,0 }; // pPosV

		Vector3Short _rawscale = { 15360, 15360, 15360 }; // new in v53
		mstudioanim_valueptr_t_v49 _animscale = { 0,0,0 };

		int _nextOffset = (int)sections[i].nextoffset > 0 ? (int)sections[i].nextoffset + (32 - GetAnimHeaderSize((int)sections[i].flags)) : 0;

		int bone = (int)sections[i].bone;
		_bone = sections[i].bone;
		_flags = (std::byte)ConvertFlag((int)sections[i].flags);

		if ((int)sections[i].flags == 12) _posscale = GetLargestNumber(lbd.posScale[bone].x, lbd.posScale[bone].y, lbd.posScale[bone].z);
		if ((int)sections[i].flags & STUDIO_ANIM_ANIMROT)
		{
			Vector3Short rotPtr = sections[i].animrot.offset;
			if (!hasAnimPosPtr)
			{
				rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
				sections[i].animrot.offset = rotPtr;
			}
			else
			{
				rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
				sections[i].animrot.offset = rotPtr;
			}
			_animrot = sections[i].animrot;

		}

		if ((int)sections[i].flags & STUDIO_ANIM_ANIMPOS)
		{
			Vector3Short posPtr = sections[i].animpos.offset;
			if (!hasAnimRotPtr)
			{
				posPtr.x > 0 ? posPtr.x += 18 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 18 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 18 : posPtr.z = 0;
				sections[i].animpos.offset = posPtr;
			}
			else
			{
				posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
				sections[i].animpos.offset = posPtr;
			}
			_animpos = sections[i].animpos;
		}

		if (hasRawRot) _rawrot = sections[i].rawrot;

		if (hasRawRot2) _rawrot = sections[i].rawrot2;

		if (hasRawPos) _rawpos = sections[i].rawpos;
		else
			_rawpos = { (short)float_to_half(lbd.bonePos[bone].x), (short)float_to_half(lbd.bonePos[bone].y), (short)float_to_half(lbd.bonePos[bone].z) };

		mstudioanim_t_v53 v53Anim{ _posscale, _bone, _flags, _unk, _rawrot, _animrot, _unused, _rawpos, _animpos, _rawscale, _animscale, _nextOffset, sections[i].animdata };
		v53Sections.push_back(v53Anim);
	}
	return v53Sections;
}

std::vector<mstudioikchain_t_v53> MDL::v52Mdl::IkChainConversion()
{
	std::vector<mstudioikchain_t_v53> v53IkChains;
	for (int i = 0; i < ikchains.size(); i++)
	{
		int stairs = 16 * (ikchains.size() - i);
		int offset = bytesAddedToTextures + textureFiller + stairs + bytesAddedToRuiMesh + bytesAddedToLinearBone;
		float unk = (float)0.707;
		if (ikchains[i].sznameindex > 0) ikchains[i].sznameindex += offset;
		if (ikchains[i].linkindex > 0) ikchains[i].linkindex += stairs;

		mstudioikchain_t_v53 ikChain = { ikchains[i].sznameindex, ikchains[i].linktype, ikchains[i].numlinks, ikchains[i].linkindex, unk, 0, 0, 0 };
		v53IkChains.push_back(ikChain);
	}
	return v53IkChains;
}

void MDL::v52Mdl::HitboxSetConversion()
{
	int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone;
	for (int i = 0; i < hitboxsets.size(); i++)
	{
		hitboxsets[i].sznameindex += stairs;
	}
}

void MDL::v52Mdl::AttachmentConversion()
{
	int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone;
	for (int i = 0; i < attachments.size(); i++)
	{
		attachments[i].sznameindex += stairs;
	}
}

void MDL::v52Mdl::SeqEventConversion()
{
	int eventNum = 0;
	for (int i = 0; i < seqdescs.size(); i++)
	{
		for (int j = 0; j < seqdescs[i].numevents; j++)
		{
			int stairs = bytesAddedToRuiMesh + bytesAddedToTextures + bytesAddedToIkChains + textureFiller + bytesAddedToLinearBone;
			if (events[eventNum].szeventindex > 0) events[eventNum].szeventindex += stairs;
			eventNum++;
		}
	}
}

void MDL::v52Mdl::ConvertNodeNames()
{
	for (int i = 0; i < nodenames.size(); i++)
	{
		if (nodenames[i].sznameindex > 0) nodenames[i].sznameindex += allBytesAdded + bytesAddedToRuiMesh;
	}
}

void MDL::v52Mdl::ConvertBodyParts()
{
	int stairs = bytesAddedToTextures + bytesAddedToIkChains + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone;
	for (int i = 0; i < bodyparts.size(); i++)
	{
		if (bodyparts[i].sznameindex > 0) bodyparts[i].sznameindex += stairs;
	}
}

void MDL::v52Mdl::ConvertModels()
{
	for (int i = 0; i < models.size(); i++)
	{
		models[i].eyeballindex = 0;
		models[i].unused[0] = 0;
		models[i].unused[1] = 0;
		if (mdlhdr.numikchains > 0)
		{
			if (models[i].meshindex > 0) models[i].meshindex += bytesAddedToIkChains;
			if (models[i].attachmentindex > 0) models[i].attachmentindex += bytesAddedToIkChains;
		}
	}
}

void MDL::v52Mdl::ConvertMeshes()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		if (mdlhdr.numikchains > 0)meshes[i].modelindex -= bytesAddedToIkChains;
		else break;
	}
}

void MDL::v52Mdl::ConvertPoseParamDescs()
{
	int stairs = bytesAddedToTextures + textureFiller + bytesAddedToRuiMesh;
	for (int i = 0; i < poseparamdescs.size(); i++)
	{
		if (poseparamdescs[i].sznameindex > 0) poseparamdescs[i].sznameindex += stairs + bytesAddedToLinearBone;
	}
}

void MDL::v52Mdl::ConvertIncludeModel()
{
	int stairs = bytesAddedToTextures + textureFiller + bytesAddedToRuiMesh;
	for (int i = 0; i < includedmodels.size(); i++)
	{
		if (includedmodels[i].sznameindex > 0) includedmodels[i].sznameindex += stairs + bytesAddedToLinearBone;
	}
}

std::vector<mstudiotexture_t_v53> MDL::v52Mdl::ConvertTextures()
{
	std::vector<mstudiotexture_t_v53> v53Textures;

	for (int i = 0; i < textures.size(); i++)
	{
		int stairs = -((20) * (textures.size() - i));
		textures[i].sznameindex += stairs + bytesAddedToLinearBone;

		mstudiotexture_t_v53 texture = { textures[i].sznameindex, textures[i].flags, textures[i].used, textures[i].unused1, 0,0,0,0,0,0,0 };
		v53Textures.push_back(texture);
	}

	return v53Textures;
}

void MDL::v52Mdl::ConvertCDTextures()
{
	for (int i = 0; i < cdtextures.size(); i++)
	{
		cdtextures[i].sznameindex += allBytesAdded + bytesAddedToRuiMesh + bytesAddedToLinearBone;
	}
}

void MDL::v52Mdl::ConvertSrcBoneTransforms()
{
	for (int i = 0; i < srcbonetransforms.size(); i++)
	{
		srcbonetransforms[i].sznameindex += bytesAddedToLinearBone;
	}
}

void MDL::v52Mdl::ConvertLinearBone()
{
	linearbone.posscaleindex = 0;
	linearbone.rotscaleindex += bytesAddedToLinearBone;
	linearbone.qalignmentindex += bytesAddedToLinearBone;
}

mstudiolinearbonedata_t_v53 MDL::v52Mdl::ConvertLinearBoneData()
{
	mstudiolinearbonedata_t_v53 linearBoneData{};
	memcpy(linearBoneData.flags, linearbonedata.flags, sizeof(uint32_t) * mdlhdr.numbones);
	memcpy(linearBoneData.parents, linearbonedata.parents, sizeof(uint32_t) * mdlhdr.numbones);
	memcpy(linearBoneData.bonePos, linearbonedata.bonePos, sizeof(Vector3) * mdlhdr.numbones);
	memcpy(linearBoneData.boneQuat, linearbonedata.boneQuat, sizeof(Quaternion) * mdlhdr.numbones);
	memcpy(linearBoneData.boneRot, linearbonedata.boneRot, sizeof(RadianEuler) * mdlhdr.numbones);
	memcpy(linearBoneData.poseToBone, linearbonedata.poseToBone, sizeof(matrix3x4_t) * mdlhdr.numbones);
	memcpy(linearBoneData.rotScale, linearbonedata.rotScale, sizeof(Vector3) * mdlhdr.numbones);
	memcpy(linearBoneData.boneAlignment, linearbonedata.boneAlignment, sizeof(Quaternion) * mdlhdr.numbones);
	return linearBoneData;
}

void MDL::v52Mdl::UpdateMdl()
{
	if (hitboxsets.size() > 0) HitboxSetConversion();
	if (attachments.size() > 0) AttachmentConversion();
	if (events.size() > 0) SeqEventConversion();
	if (mdlhdr.numlocalnodes > 0) ConvertNodeNames();
	if (mdlhdr.numbodyparts > 0) ConvertBodyParts();
	if (models.size() > 0) ConvertModels();
	if (meshes.size() > 0) ConvertMeshes();
	if (mdlhdr.numlocalposeparameters > 0) ConvertPoseParamDescs();
	if (mdlhdr.numincludemodels > 0) ConvertIncludeModel();
	if (mdlhdr.numcdtextures > 0) ConvertCDTextures();
	if (mdlsubhdr.numsrcbonetransform > 0) ConvertSrcBoneTransforms();
	if (mdlhdr.numbones > 0) ConvertLinearBone();
}

#pragma endregion

#pragma region MDLV49

std::vector <int> MDL::v49Mdl::GetIKBones()
{
	std::vector <int> chain;
	std::vector <int> chainBones;
	std::vector <int> roots;

	for (int i = 0; i < iklinks.size(); i++)
	{
		mstudioiklink_t_v49 iklink = iklinks[i];
		roots.push_back(iklink.bone);
	}

	for (int i = 0; i < roots.size(); i++)
	{
//		Logger::Info("Root Chain Bone Read: %d\n", roots[i]);
		chain.push_back(roots[i]);
	}

	for (int i = 0; i < bones.size(); i++)
	{
		mstudiobone_t_v49 _bone = bones[i];

		if ( contains(roots, _bone.parent) && !contains(chain, i))
		{
			chain.push_back(i);
//			Logger::Info("Child of Root Chain Bone Read: %d | Root: %d\n", i, _bone.parent);
		}

		if (contains(chain, _bone.parent) && !contains(chain, i))
		{
			chain.push_back(i);
//			Logger::Info("Child of Root Chain Bone Child Read: %d | Parent: %d\n", i, _bone.parent);
		}

	}

	for (int i = 0; i < chain.size(); i++)
	{
		if (!contains(chainBones, chain[i]))
		{
			chainBones.push_back(chain[i]);
		}
	}
//	Logger::Info("Total ChainBones: %d\n", chainBones.size());

	return chainBones;
}

void MDL::v49Mdl::v49ExtractSrcBoneTransforms(BinaryReader* Stream)
{
	if (mdlsubhdr.numsrcbonetransform > 0)
	{
		std::string fileName = Stream->_filename;
		fileName.resize(fileName.size() - 4);
		fileName.insert(fileName.size(), ".srcbtf");
		BinaryWriter Bing = BinaryWriter(fileName.c_str());

		for (int i = 0; i < mdlsubhdr.numsrcbonetransform; i++)
		{
			Bing.Write(srcbonetransforms[i]);
		}
	}
}

MDL::v49Mdl::v49Mdl()
{

}

MDL::v49Mdl::v49Mdl(studiohdr_t_v49 _mdlhdr, studiohdr2_t_v49 _mdlsubhdr, std::vector<mstudiobone_t_v49> _bones, std::vector<mstudiojigglebone_t_v49> _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, std::vector<mstudioattachment_t_v49> _attachments, std::vector<mstudiohitboxset_t_v49> _hitboxsets, std::vector<mstudiobbox_t_v49> _hitboxes, mstudiobonenametable_t_v49 _bonenametable, std::vector<mstudioanimdesc_t_v49> _animdescs, std::vector<mstudioanim_t_v49> _anims, std::vector<sectionindexesindex_t_v49> _sectionindexes, std::vector<mstudioanim_t_v49> _sections, std::vector<mstudioikrule_t_v49> _ikrules, std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors, std::vector<mstudioikerror_t_v49> _ikerrors, std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe, std::vector<mstudioseqdescv49_t> _seqdescs, std::vector<blendgroup_t_v49> _blends, std::vector<posekey_t_v49> _posekeys, std::vector<mstudioevent_t_v49> _events, std::vector<mstudioautolayer_t_v49> _autolayers, std::vector<mstudioactivitymodifier_t_v49> _activitymodifiers, std::vector<seqweightlist_t_v49> _seqweightlist, std::vector<mstudionodename_t_v49> _nodenames, std::vector<mstudionodedata_v49> _nodes, std::vector<mstudiobodyparts_t_v49> _bodyparts, std::vector<mstudiomodel_t_v49>	_models, std::vector<mstudiomesh_t_v49> _meshes, std::vector<mstudioikchain_t_v49> _ikchains, std::vector<mstudioiklink_t_v49> _iklinks, std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs, std::vector<mstudioanimblock_t> _animblocks, std::vector<mstudiomodelgroup_t_v49> _includedmodels, std::vector<mstudiotexturedir_t_v49> _cdtextures, std::vector<mstudiotexture_t_v49> _textures, std::vector<mstudioskingroup_t_v49> _skingroups, mstudiokeyvalues_t_v49 _keyvalues, std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v49 _linearbonedata, mstudiostringtable_t_v49 _stringtable)
{
	mdlhdr = _mdlhdr;
	mdlsubhdr = _mdlsubhdr;
	bones = _bones;
	jigglebones = _jigglebones;
	boneflexdrivers = _boneflexdrivers;
	attachments = _attachments;
	hitboxsets = _hitboxsets;
	hitboxes = _hitboxes;
	bonenametable = _bonenametable;
	animdescs = _animdescs;
	anims = _anims;
	sectionindexes = _sectionindexes;
	sections = _sections;
	ikrules = _ikrules;
	compressedikerrors = _compressedikerrors;
	ikerrors = _ikerrors;
	ikrulezeroframe = _ikrulezeroframe;
	seqdescs = _seqdescs;
	blends = _blends;
	posekeys = _posekeys;
	events = _events;
	autolayers = _autolayers;
	activitymodifiers = _activitymodifiers;
	seqweightlist = _seqweightlist;
	nodenames = _nodenames;
	nodes = _nodes;
	bodyparts = _bodyparts;
	models = _models;
	meshes = _meshes;
	ikchains = _ikchains;
	iklinks = _iklinks;
	poseparamdescs = _poseparamdescs;
	animblocks = _animblocks;
	includedmodels = _includedmodels;
	cdtextures = _cdtextures;
	textures = _textures;
	skingroups = _skingroups;
	keyvalues = _keyvalues;
	srcbonetransforms = _srcbonetransforms;
	linearbone = _linearbone;
	linearbonedata = _linearbonedata;
	stringtable = _stringtable;
}

MDL::v49Mdl MDL::v49Mdl::_v49Mdl(BinaryReader* Stream, bool debug)
{
	Stream->seek(0);
#pragma region VARS
	studiohdr_t_v49	 mdlhdr; Stream->Read(&mdlhdr);
	studiohdr2_t_v49 mdlsubhdr; Stream->Read(&mdlsubhdr);

	std::vector<mstudiobone_t_v49>						bones;
	std::vector<mstudiojigglebone_t_v49>				jigglebones;
	std::vector<mstudioboneflexdriver_t_v49>			boneflexdrivers;
	std::vector<mstudioattachment_t_v49>				attachments;
	std::vector<mstudiohitboxset_t_v49>					hitboxsets;
	std::vector<mstudiobbox_t_v49>						hitboxes;
	mstudiobonenametable_t_v49							bonenametable;
	std::vector<mstudioanimdesc_t_v49>					animdescs;
	std::vector<mstudioanim_t_v49>						anims;
	std::vector<sectionindexesindex_t_v49>				sectionindexes;
	std::vector<mstudioanim_t_v49>						sections;
	std::vector<mstudioikrule_t_v49>					ikrules;
	std::vector<mstudiocompressedikerror_t_v49>			compressedikerrors;
	std::vector<mstudioikerror_t_v49>					ikerrors;
	std::vector<mstudioikrulezeroframe_t_v49>			ikrulezeroframe;
	std::vector<mstudioseqdescv49_t>					seqdescs;
	std::vector<blendgroup_t_v49>						blends;
	std::vector<posekey_t_v49>							posekeys;
	std::vector<mstudioevent_t_v49>						events;
	std::vector<mstudioautolayer_t_v49>					autolayers;
	std::vector<mstudioactivitymodifier_t_v49>			activitymodifiers;
	std::vector<seqweightlist_t_v49>					seqweightlist;
	std::vector<mstudionodename_t_v49>					nodenames;
	std::vector<mstudionodedata_v49>					nodes;
	std::vector<mstudiobodyparts_t_v49>					bodyparts;
	std::vector<mstudiomodel_t_v49>						models;
	std::vector<mstudiomesh_t_v49>						meshes;
	std::vector<mstudioikchain_t_v49>					ikchains;
	std::vector<mstudioiklink_t_v49>					iklinks;
	std::vector<mstudioposeparamdesc_t_v49>				poseparamdescs;
	std::vector<mstudioanimblock_t>						animBlocks;
	std::vector<mstudiomodelgroup_t_v49>				includedmodels;
	std::vector<mstudiotexturedir_t_v49>				cdtextures;
	std::vector<mstudiotexture_t_v49>					textures;
	std::vector<mstudioskingroup_t_v49>					skingroups;
	mstudiokeyvalues_t_v49								keyvalues;
	std::vector<mstudiosrcbonetransform_t_v49>			srcbonetransforms;
	mstudiolinearbone_t_v49								linearbone;
	mstudiolinearbonedata_t_v49							linearbonedata;

	std::vector<int>									weightlistidxs;

	std::vector<int>									bytesAddedToWeightLists;

//	int weightListPos = 0;

	int numjigglebones = 0;

	int numhitboxes = 0;

	int nummodels = 0;

	int nummeshes = 0;

	int numiklinks = 0;

	int numiklocks = 0;

	int numikrules = 0;

#pragma endregion
#pragma region BONES
	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlhdr.boneindex);
		for (int i = 0; i < mdlhdr.numbones; i++)
		{
			int bonePos = mdlhdr.boneindex + 216 * i;
			Stream->seek(bonePos);

			mstudiobone_t_v49 bone; Stream->Read(&bone);
			bones.push_back(bone);
			if (bone.proctype == 5) numjigglebones++;

			std::string szName = Stream->ReadNullTermStrTrgt(bonePos + bone.sznameindex, false);

			Logger::Info("Bone Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region JIGGLEBONES
	if (numjigglebones > 0)
	{
		int jiggleBonePos = mdlhdr.boneindex + 216 * mdlhdr.numbones;

		Stream->seek(jiggleBonePos);

		for (int i = 0; i < numjigglebones; i++)
		{
			mstudiojigglebone_t_v49 jigglebone; Stream->Read(&jigglebone);
			Logger::Info("JiggleBoneFlags Read: %s\n", Utility::func::ReadJiggleFlags(jigglebone.flags).c_str());
			jigglebones.push_back(jigglebone);
			Logger::Info("JiggleBone Read: %d\n", i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
#pragma endregion
#pragma region ATTACHMENTS
	if (mdlhdr.numlocalattachments > 0)
	{
		Stream->seek(mdlhdr.localattachmentindex);
		for (int i = 0; i < mdlhdr.numlocalattachments; i++)
		{
			int attachmentPos = mdlhdr.localattachmentindex + 92 * i;
			Stream->seek(attachmentPos);

			mstudioattachment_t_v49 attachment; Stream->Read(&attachment);

			std::string szName = Stream->ReadNullTermStrTrgt(attachmentPos + attachment.sznameindex, false);
			attachment.szname = szName;

			attachments.push_back(attachment);
			Logger::Info("Attachment Read: %s : %d\n", attachment.szname.c_str(), i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
#pragma endregion
#pragma region HITBOXSETS
	if (mdlhdr.numhitboxsets > 0)
	{
		Stream->seek(mdlhdr.hitboxsetindex);
		for (int i = 0; i < mdlhdr.numhitboxsets; i++)
		{
			int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * i;
			Stream->seek(hitboxsetPos);

			mstudiohitboxset_t_v49 hitboxset; Stream->Read(&hitboxset);

			if (hitboxset.numhitboxes > 0) numhitboxes += hitboxset.numhitboxes;

			Stream->seek(hitboxsetPos + hitboxset.hitboxindex);
			for (int j = 0; j < hitboxset.numhitboxes; j++)
			{
				int hitboxPos = hitboxsetPos + hitboxset.hitboxindex + 68 * i;
				Stream->seek(hitboxPos);
				mstudiobbox_t_v49 hitbox; Stream->Read(&hitbox);

				std::string szHitboxName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.szhitboxnameindex, false);
				hitbox.szhitboxname = szHitboxName;

				hitboxset.hitboxes.push_back(hitbox);
			}

			std::string szName = Stream->ReadNullTermStrTrgt(hitboxsetPos + hitboxset.sznameindex, false);
			hitboxset.szname = szName;

			hitboxsets.push_back(hitboxset);
			Logger::Info("HitboxSet Read: %s : %d\n", szName.c_str(), i);
		}
#pragma endregion
#pragma region HITBOXES
		if (numhitboxes > 0)
		{
			for (int i = 0; i < numhitboxes; i++)
			{
				int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * mdlhdr.numhitboxsets;
				int hitboxPos = hitboxsetPos + 68 * i;
				Stream->seek(hitboxPos);
				mstudiobbox_t_v49 hitbox; Stream->Read(&hitbox);

				std::string szName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.szhitboxnameindex, false);
				hitbox.szhitboxname = szName;

				hitboxes.push_back(hitbox);
				Logger::Info("Hitbox Read: %s : %d\n", hitbox.szhitboxname.c_str(), i);
			}
		}
		//Logger::Info("strPos: %d\n", Stream->Position());

	}
#pragma endregion
#pragma region BONENAMETABLE
	if (mdlhdr.numbones > 0)
	{
		Stream->Read(&bonenametable, mdlhdr.numbones);
		Logger::Info("BoneTable Read: %d\n", mdlhdr.numbones);
	}
#pragma endregion
#pragma region ANIMDESCS
	if (mdlhdr.numlocalanim > 0)
	{
		Stream->seek(mdlhdr.localanimindex);
		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int animDescPos = mdlhdr.localanimindex + 100 * i;
			Stream->seek(animDescPos);

			mstudioanimdesc_t_v49 animdesc; Stream->Read(&animdesc);

			std::string szName = Stream->ReadNullTermStrTrgt(animDescPos + animdesc.sznameindex, false);
			animdesc.szname = szName;

			animdescs.push_back(animdesc);

			Logger::Info("AnimDesc Read: %s : %d\n", szName.c_str(), i);
			//Logger::Info("strPos: %d\n", Stream->Position());
		}
#pragma endregion
#pragma region ANIMS
		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int boneHeaderNum = 0;
			int animDescPos = mdlhdr.localanimindex + 100 * i;
			int animPos = animDescPos + animdescs[i].animindex;

			if (animdescs[i].sectionindex == 0)
			{

				Stream->seek(animPos);
				//Logger::Info("strPos: %d\n", Stream->Position());
				for (int j = 0; j < mdlhdr.numlocalanim * mdlhdr.numbones; j++)
				{
					int dist = 0;
					int nextAnim = i + 1;
					int hdrStartPos = Stream->Position();
					//Logger::Info("hdrStartPos: %d\n", hdrStartPos);
					mstudioanim_t_v49 anim; Stream->Read(&anim);
					mstudioanimdata_t_v49 animData;

					//Logger::Info("HdrNum: %d\n", j);
					//Logger::Info("Bone: %d\n", anim.bone);
					//Logger::Info("Flags Read: %d\n", anim.flags);
					//Logger::Info("NextOffset Read: %d\n", anim.nextoffset);
					//Logger::Info("RawRot Read: One: %d I: %d J: %d K: %d\n", anim.rawrot.one, anim.rawrot.i, anim.rawrot.j, anim.rawrot.k);
					//Logger::Info("RawRot2 Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot2.x, anim.rawrot2.y, anim.rawrot2.z, anim.rawrot2.wneg);
					//Logger::Info("RawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
					//Logger::Info("RotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
					//Logger::Info("PosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);

					int headerSize = GetAnimHeaderSize((int)anim.flags);
					int hdrEndPos = Stream->Position();
					//Logger::Info("hdrEndPos: %d\n", hdrEndPos);
					boneHeaderNum++;
					if (anim.nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - hdrEndPos;
						if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - hdrEndPos;

						//Logger::Info("SeqIndx: %d\n", mdlhdr.localseqindex);
						//Logger::Info("AnimDataSizeEnd: %d\n", dist);
						Stream->Read(&animData, dist);
						anim.animdata = animData;
						anims.push_back(anim);
						Logger::Info("Anim Read: %d\n", i);
						break;
					}
					dist = anim.nextoffset - headerSize;
					//Logger::Info("AnimDataSize: %d\n", dist);
					Stream->Read(&animData, dist);
					anim.animdata = animData;
					anims.push_back(anim);

				}

			}
#pragma endregion
#pragma region SECTIONS
			if (animdescs[i].sectionindex > 0)
			{
				int secBoneHeaderNum = 0;
				int animDescPos = mdlhdr.localanimindex + 100 * i;
				int animPos = animDescPos + animdescs[i].animindex;
				int secPos = animDescPos + animdescs[i].sectionindex;
				int frames = animdescs[i].numframes;
				int secFrames = animdescs[i].sectionframes;
				std::vector<sectionindexesindex_t_v49> idxs;

				int secNumber = (frames / secFrames) + 2;

				Stream->seek(secPos);

				for (int j = 0; j < secNumber; j++)
				{

					sectionindexesindex_t_v49 sectionIdx; Stream->Read(&sectionIdx);
					//Logger::Info("secIdx: %d\n", sectionIdx.sectionoffset);
					idxs.push_back(sectionIdx);
					sectionindexes.push_back(sectionIdx);
				}

				for (int j = 0; j < secNumber; j++)
				{
					sectionindexesindex_t_v49 sectionIdx = idxs[j];
					int nextSection = j + 1;
					//Logger::Info("secIdxPos: %d\n", animDescPos + sectionIdx.sectionoffset);
					Stream->seek(animDescPos + sectionIdx.sectionoffset);


					for (int k = 0; k < mdlhdr.numlocalanim * mdlhdr.numbones; k++)
					{
						int dist = 0;
						int nextAnim = i + 1;
						int hdrStartPos = Stream->Position();
						//Logger::Info("secHdrStartPos: %d\n", hdrStartPos);
						mstudioanim_t_v49 anim; Stream->Read(&anim);
						mstudioanimdata_t_v49 animData;

						//Logger::Info("SecHdrNum: %d\n", j);
						//Logger::Info("SecBone: %d\n", anim.bone);
						//Logger::Info("SecFlags Read: %d\n", anim.flags);
						//Logger::Info("SecNextOffset Read: %d\n", anim.nextoffset);
						//Logger::Info("SecRawRot Read: One: %d I: %d J: %d K: %d\n", anim.rawrot.one, anim.rawrot.i, anim.rawrot.j, anim.rawrot.k);
						//Logger::Info("SecRawRot2 Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot2.x, anim.rawrot2.y, anim.rawrot2.z, anim.rawrot2.wneg);
						//Logger::Info("SecRawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
						//Logger::Info("SecRotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
						//Logger::Info("SecPosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);

						int headerSize = GetAnimHeaderSize((int)anim.flags);
						int hdrEndPos = Stream->Position();
						//Logger::Info("secHdrEndPos: %d\n", hdrEndPos);
						secBoneHeaderNum++;
						if (anim.nextoffset == 0)
						{
							if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - hdrEndPos;  //Logger::Info("Distance from sectionIdx: %d\n", strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - hdrEndPos;
							if (nextSection < secNumber && nextAnim > mdlhdr.numlocalanim - 1 && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + idxs[nextSection].sectionoffset - hdrEndPos;
							if (nextSection > secNumber && nextAnim > mdlhdr.numlocalanim - 1 && animdescs[i].numikrules == 0) dist = mdlhdr.localseqindex - hdrEndPos;
							Stream->Read(&animData, dist);
							anim.animdata = animData;
							sections.push_back(anim);
							//Logger::Info("AnimDataSizeEnd: %d\n", dist);
							Logger::Info("Section Read: %d\n", i);
							break;
						}
						dist = anim.nextoffset - headerSize;
						Stream->Read(&animData, dist);
						anim.animdata = animData;
						sections.push_back(anim);
					}

				}

			}
#pragma endregion
#pragma region IKRULES
			if (animdescs[i].numikrules > 0)
			{
				int numOfCompressedIkErrors = 0;
				int numOfIkErrors = 0;
				std::vector<int> errorIdxs;
				int nextAnim = i + 1;

				for (int j = 0; j < animdescs[i].numikrules; j++)
				{
					Stream->seek(animDescPos + animdescs[i].ikruleindex + 152 * j);
					int pos = Stream->Position();
					mstudioikrule_t_v49 v49IkRule; Stream->Read(&v49IkRule);
					ikrules.push_back(v49IkRule);
					if (v49IkRule.compressedikerrorindex > 0) errorIdxs.push_back(pos + v49IkRule.compressedikerrorindex);
					if (v49IkRule.compressedikerrorindex > 0) numOfCompressedIkErrors++;
					if (v49IkRule.ikerrorindex > 0) numOfIkErrors++;

					Logger::Info("IkRule Read: %d\n", j);
					numikrules++;
				}
#pragma endregion
#pragma region COMPRESSEDIKERRORS
				if (numOfCompressedIkErrors > 0)
				{
					int animStartPos = mdlhdr.localanimindex + 100 * i;
					for (int j = 0; j < numOfCompressedIkErrors; j++)
					{
						Stream->seek(errorIdxs[j]);
						int dist = 0;
						int next = j + 1;
						int pos = Stream->Position();
						mstudiocompressedikerror_t_v49 v49CompressedIkError; Stream->Read(&v49CompressedIkError);
						mstudioanimdata_t_v49 animData;
						int endPos = Stream->Position();
						if (j + 1 > numOfCompressedIkErrors - 1)
						{
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0)  dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - endPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - endPos;
							if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - endPos;
							Stream->Read(&animData, dist);
							v49CompressedIkError.animdata = animData;
							compressedikerrors.push_back(v49CompressedIkError);
							Logger::Info("CompressedIkError Read: %d\n", j);
							break;
						}
//						Logger::Info("CompressedIkError Read: %d\n", j);
						dist = errorIdxs[next] - endPos;
						Stream->Read(&animData, dist);
						v49CompressedIkError.animdata = animData;
						compressedikerrors.push_back(v49CompressedIkError);
					}
				}

			}

		}
	}
#pragma endregion
#pragma region SEQDESCS
	#pragma region SEQDESC
		if (mdlhdr.numlocalseq > 0)
		{
			int actModNum = 0;
			int eventNum = 0;
	
			Stream->seek(mdlhdr.localseqindex);
			for (int i = 0; i < mdlhdr.numlocalseq; i++)
			{
				int seqPos = mdlhdr.localseqindex + 212 * i;
				Stream->seek(seqPos);
				mstudioseqdescv49_t seqDesc; Stream->Read(&seqDesc);
				seqDesc.szlabel = Stream->ReadNullTermStrTrgt(seqPos + seqDesc.szlabelindex, false);
				seqDesc.szactivityname = Stream->ReadNullTermStrTrgt(seqPos + seqDesc.szactivitynameindex, false);//ReadMDLString(Stream, seqPos + seqDesc.szactivitynameindex);
	
				seqdescs.push_back(seqDesc);
	
				Logger::Info("Seq Read: %s : %s : %d\n", seqDesc.szlabel.c_str(), seqDesc.szactivityname.c_str(), i);
			}
	#pragma endregion
	#pragma region SEQDATA
			for (int i = 0; i < mdlhdr.numlocalseq; i++)
			{
				#pragma region VARS
							int seqPos = mdlhdr.localseqindex + 212 * i;
							int numBlends = seqdescs[i].groupsize[0] * seqdescs[i].groupsize[1];
							int posekeySize = seqdescs[i].groupsize[0] + seqdescs[i].groupsize[1];
							int numAutolayers = seqdescs[i].numautolayers;
							int numActMods = seqdescs[i].numactivitymodifiers;
							int numEvents = seqdescs[i].numevents;
				#pragma endregion
				#pragma region WEIGHTLIST
							if (seqdescs[i].weightlistindex > 0)
							{
								Stream->seek(seqPos + seqdescs[i].weightlistindex);
								seqweightlist_t_v49 weightlist; Stream->Read(&weightlist, mdlhdr.numbones);
								seqweightlist.push_back(weightlist);
								Logger::Info("Weightlist Read: %d\n", i);
							}
				#pragma endregion
				#pragma region BLENDS
							if (seqdescs[i].animindexindex > 0)
							{
								Stream->seek(seqPos + seqdescs[i].animindexindex);
								blendgroup_t_v49 blendgroup; Stream->Read(&blendgroup, numBlends);
								blends.push_back(blendgroup);
								seqdescs[i].blends = blendgroup;
								Logger::Info("BlendGroup Read: %d\n", i);
							}
				#pragma endregion
				#pragma region POSEKEY
							if (seqdescs[i].posekeyindex > 0)
							{
								Stream->seek(seqPos + seqdescs[i].posekeyindex);
								posekey_t_v49 posekey; Stream->Read(&posekey, posekeySize);
								if (posekeySize == 3)posekey.unk[2] == 0.0;
				//				Logger::Info("PosekeyF Read: %f\n", posekey.unk[2]);
								posekeys.push_back(posekey);
								Logger::Info("Posekey Read: %d\n", i);
							}
				#pragma endregion
				#pragma region AUTOLAYER
							if (numAutolayers > 0)
							{
								Stream->seek(seqPos + seqdescs[i].autolayerindex);
								for (int j = 0; j < numAutolayers; j++)
								{
									mstudioautolayer_t_v49 autolayer; Stream->Read(&autolayer);
									autolayers.push_back(autolayer);
									Logger::Info("Autolayer Read: %d\n", j);
								}
							}
				#pragma endregion
				#pragma region EVENTS
							if (numEvents > 0)
							{
								Stream->seek(seqPos + seqdescs[i].eventindex);
								for (int j = 0; j < seqdescs[i].numevents; j++)
								{
									mstudioevent_t_v49 _event; Stream->Read(&_event);
									events.push_back(_event);
				
									std::string szEventName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].eventindex + 80 * j + _event.szeventindex, false);
									std::string szName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].eventindex + 80 * j + 12, false);
				
									seqdescs[i].szeventnames.push_back(szEventName);
									seqdescs[i].events.push_back(eventNum);
				
									Logger::Info("Event Read: %s : %s : %d\n", szName.c_str(), szEventName.c_str(), j);
									eventNum++;
								}
							}
				#pragma endregion
				#pragma region ACTIVITYMODIFIERS
							if (numActMods > 0)
							{
								Stream->seek(seqPos + seqdescs[i].activitymodifierindex);
								for (int j = 0; j < seqdescs[i].numactivitymodifiers; j++)
								{
									mstudioactivitymodifier_t_v49 actmod; Stream->Read(&actmod);
									
									std::string szName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].activitymodifierindex + 4 * j + actmod.sznameindex, false);
				
									seqdescs[i].szactivitymodifiernames.push_back(szName);
									actmod.szname = szName;
									activitymodifiers.push_back(actmod);
									seqdescs[i].actmods.push_back(actModNum);
									Logger::Info("ActMod Read: %s : %d\n", seqdescs[i].szactivitymodifiernames[j].c_str(), j);
									actModNum++;
								}
							}
				#pragma endregion
			}
		}
	#pragma endregion
#pragma endregion
#pragma region NODENAMES
	if (mdlhdr.numlocalnodes > 0)
	{
		Stream->seek(mdlhdr.localnodenameindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodename_t_v49 nodeName; Stream->Read(&nodeName);

			std::string szName = Stream->ReadNullTermStrTrgt(nodeName.sznameindex, false);
			nodeName.szname = szName;
			nodenames.push_back(nodeName);
			Logger::Info("NodeName Read: %s : %d\n", nodeName.szname.c_str(), i);
		}
#pragma endregion
#pragma region NODEDATA
		Stream->seek(mdlhdr.localnodeindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodedata_v49 node; Stream->Read(&node, mdlhdr.numlocalnodes * mdlhdr.numlocalnodes);
			nodes.push_back(node);
			Logger::Info("Node Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region BODYPARTS
	if (mdlhdr.numbodyparts > 0)
	{
		Stream->seek(mdlhdr.bodypartindex);
		for (int i = 0; i < mdlhdr.numbodyparts; i++)
		{
			mstudiobodyparts_t_v49 bodyPart; Stream->Read(&bodyPart);
			if (bodyPart.nummodels > 0) nummodels += bodyPart.nummodels;

			std::string szName = Stream->ReadNullTermStrTrgt(mdlhdr.bodypartindex + 16 * i + bodyPart.sznameindex, false);
			bodyPart.szname = szName;

			bodyparts.push_back(bodyPart);
			Logger::Info("Bodypart Read: %s : %d\n", bodyPart.szname.c_str(), i);
		}
	}
#pragma endregion
#pragma region MODELS
	if (nummodels > 0)
	{
		Stream->seek(mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts);
		for (int i = 0; i < nummodels; i++)
		{
			mstudiomodel_t_v49 model; Stream->Read(&model);
			if (model.nummeshes > 0) nummeshes += model.nummeshes;
			models.push_back(model);
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts + 148 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(modelPos, false);

			Logger::Info("Model Read: %s : %d\n", szName.c_str(), i);
		}

	}
#pragma endregion
#pragma region IKCHAINS
	if (mdlhdr.numikchains > 0)
	{
		std::vector <int> ikRoots;
		Stream->seek(mdlhdr.ikchainindex);
		for (int i = 0; i < mdlhdr.numikchains; i++)
		{
			mstudioikchain_t_v49 ikchain; Stream->Read(&ikchain);
			if (ikchain.numlinks > 0) numiklinks += ikchain.numlinks;

			int ikChainPos = mdlhdr.ikchainindex + 16 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(ikChainPos + ikchain.sznameindex, false);
			ikchain.szname = szName;

			ikchains.push_back(ikchain);
			Logger::Info("IkChain Read: %s : %d\n", szName.c_str(), i);
		}
#pragma endregion
#pragma region IKLINKS
		for (int i = 0; i < numiklinks; i++)
		{
			int ikLinkPos = (mdlhdr.ikchainindex + 16 * mdlhdr.numikchains) + 28 * i;
			Stream->seek(ikLinkPos);
			mstudioiklink_t_v49 iklink; Stream->Read(&iklink);
			iklinks.push_back(iklink);
			Logger::Info("IkLink Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region POSEPARAMDESCS
	if (mdlhdr.localposeparamindex > 0)
	{
		Stream->seek(mdlhdr.localposeparamindex);
		for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
		{
			mstudioposeparamdesc_t_v49 poseparamdesc; Stream->Read(&poseparamdesc);
			int poseParamPos = mdlhdr.localposeparamindex + 20 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(poseParamPos + poseparamdesc.sznameindex, false);
			poseparamdesc.szname = szName;

			poseparamdescs.push_back(poseparamdesc);
			Logger::Info("PoseParamDesc Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region ANIMBLOCKS
	if (mdlhdr.numanimblocks > 0)
	{
		Stream->seek(mdlhdr.localnodeindex);
		for (int i = 0; i < mdlhdr.numanimblocks; i++)
		{
			mstudioanimblock_t animBlock; Stream->Read(&animBlock);
			animblocks.push_back(animBlock);
			Logger::Info("AnimBlock Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region MESHES
	if (nummeshes > 0)
	{
		for (int i = 0; i < nummeshes; i++)
		{
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts;
			int meshPos = (modelPos + 148 * nummodels) + (20 * mdlhdr.numlocalposeparameters) + (16 * mdlhdr.numikchains + 28 * numiklinks) + 116 * i;
			Stream->seek(meshPos);
			mstudiomesh_t_v49 mesh; Stream->Read(&mesh);
			meshes.push_back(mesh);

			int texturePos = mdlhdr.textureindex + (64 * (int)mesh.material);
			Stream->seek(texturePos);
			mstudiotexture_t_v53 texture; Stream->Read(&texture);

			std::string szName = Stream->ReadNullTermStrTrgt(texturePos + texture.sznameindex, false);

			Logger::Info("MeshMat Read: %s : %d\n", szName.c_str(), i);
			Logger::Info("Mesh Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region INCLUDEMODEL
	if (mdlhdr.numincludemodels > 0)
	{
		Stream->seek(mdlhdr.includemodelindex);
		for (int i = 0; i < mdlhdr.numincludemodels; i++)
		{
			mstudiomodelgroup_t_v49 includemodel; Stream->Read(&includemodel);
			int includeModelPos = mdlhdr.includemodelindex + 8 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(includeModelPos + includemodel.sznameindex, false);
			includemodel.szname = szName;

			includedmodels.push_back(includemodel);
			Logger::Info("IncludedModel Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region TEXTURES
	if (mdlhdr.numtextures > 0)
	{
		Stream->seek(mdlhdr.textureindex);
		for (int i = 0; i < mdlhdr.numtextures; i++)
		{
			mstudiotexture_t_v49 texture; Stream->Read(&texture);
			int texturePos = mdlhdr.textureindex + 64 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(texturePos + texture.sznameindex, false);
			texture.szname = szName;

			textures.push_back(texture);

			Logger::Info("Texture Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region CDMATERIALS
	if (mdlhdr.numcdtextures > 0)
	{
		Stream->seek(mdlhdr.cdtextureindex);
		for (int i = 0; i < mdlhdr.numcdtextures; i++)
		{
			mstudiotexturedir_t_v49 cdtexture; Stream->Read(&cdtexture);

			std::string szName = Stream->ReadNullTermStrTrgt(cdtexture.sznameindex, false);
			cdtexture.szname = szName;

			cdtextures.push_back(cdtexture);
			Logger::Info("CdTexture Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region SKINFAMILIES
	if (mdlhdr.numskinfamilies > 0)
	{
		if (mdlhdr.numskinref > 0)
		{
			Stream->seek(mdlhdr.skinindex);
			for (int i = 0; i < mdlhdr.numskinfamilies; i++)
			{
				mstudioskingroup_t_v49 skingroup; Stream->Read(&skingroup, mdlhdr.numskinref);
				skingroups.push_back(skingroup);
				Logger::Info("SkinGroup Read: %d\n", i);
			}
		}
	}
#pragma endregion
#pragma region KEYVALUES
	if (mdlhdr.keyvaluesize > 0)
	{
		Stream->seek(mdlhdr.keyvalueindex);
		Stream->Read(&keyvalues, mdlhdr.keyvaluesize);
		Logger::Info("Keyvalues Read: %d\n", mdlhdr.keyvaluesize);
	}
#pragma endregion
#pragma region SRCBONETRANSFORMS
	if (mdlsubhdr.numsrcbonetransform > 0)
	{
		Stream->seek(mdlsubhdr.srcbonetransformindex);
		for (int i = 0; i < mdlsubhdr.numsrcbonetransform; i++)
		{
			mstudiosrcbonetransform_t_v49 srcbonetransform; Stream->Read(&srcbonetransform);
			int srcBonePos = mdlsubhdr.srcbonetransformindex + 100 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(srcBonePos + srcbonetransform.sznameindex, false);

			srcbonetransforms.push_back(srcbonetransform);
			Logger::Info("SrcBoneTransform Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region LINEARBONE
	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlsubhdr.linearboneindex + 408);
		Stream->Read(&linearbone);
		Stream->Read(&linearbonedata, mdlhdr.numbones);
		Logger::Info("LinearBone Read: %d\n", mdlhdr.numbones);
	}
#pragma endregion
#pragma region STRINGTABLE
	Stream->seek(mdlsubhdr.sznameindex + 408);
	mstudiostringtable_t_v49 stringTable; Stream->Read(&stringTable, mdlhdr, seqdescs, hitboxsets, attachments, nodenames, bodyparts, ikchains,animdescs, textures, includedmodels, cdtextures, poseparamdescs, srcbonetransforms);
	//std::string string(stringTable.mdlname.begin(), stringTable.mdlname.end());
	//Logger::Info("mldName: %s\n", string.c_str());
#pragma endregion

	MDL::v49Mdl _v49mdl = { mdlhdr, mdlsubhdr, bones, jigglebones, boneflexdrivers, attachments, hitboxsets, hitboxes, bonenametable, animdescs, anims, sectionindexes, sections, ikrules, compressedikerrors, ikerrors, ikrulezeroframe, seqdescs, blends, posekeys, events, autolayers, activitymodifiers, seqweightlist, nodenames, nodes, bodyparts, models, meshes, ikchains, iklinks, poseparamdescs, animBlocks, includedmodels, cdtextures, textures, skingroups, keyvalues, srcbonetransforms, linearbone, linearbonedata, stringTable };
	return _v49mdl;
}

int MDL::v49Mdl::ConvertFlag(int flag)
{
	int test = 0;
	int flagNum = 0;

	if (flag & STUDIO_ANIM_ANIMROT)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMROT);
		flagNum++;
	}
	if (flag & STUDIO_ANIM_ANIMPOS)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMPOS);
		flagNum++;
	}

	if (flag & STUDIO_ANIM_RAWROT)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT);
		test += STUDIO_ANIM_RAWROT_53;
		flagNum++;
	}
	if (flag & STUDIO_ANIM_RAWROT2)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT2);
		test += STUDIO_ANIM_RAWROT_53;
		flagNum++;
	}
	if (flag & STUDIO_ANIM_RAWPOS)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWPOS);
		test += STUDIO_ANIM_RAWPOS_53;
		flagNum++;
	}

	if (flag & STUDIO_ANIM_DELTA)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_DELTA);
		test += STUDIO_ANIM_DELTA_53;
	}

	if (flagNum == 1)
	{
		if (flag & STUDIO_ANIM_ANIMROT)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMROT);
			test += STUDIO_ANIM_RAWPOS_53;
		}
		if (flag & STUDIO_ANIM_ANIMPOS)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMPOS);
			test += STUDIO_ANIM_RAWROT_53;
		}

		if (flag & STUDIO_ANIM_RAWROT)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT);
			test += STUDIO_ANIM_RAWPOS_53;
		}
		if (flag & STUDIO_ANIM_RAWROT2)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT2);
			test += STUDIO_ANIM_RAWPOS_53;
		}
		if (flag & STUDIO_ANIM_RAWPOS)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWPOS);
			test += STUDIO_ANIM_READBONE_53;
		}
	}

	test += STUDIO_ANIM_RAWSCALE_53;
	if (flag == 0)
	{
		test = 0;
	}
	return test;
}

std::vector<int> MDL::v49Mdl::GetIkChainBones()
{
	std::vector<int> ikChainBones;
	std::vector<int> ikChainBonesChildren;
	std::vector<int> ikChainBonesFinal;

	for (int i = 0; i < iklinks.size(); i++)
	{
		ikChainBones.push_back(iklinks[i].bone);
	}

	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < ikChainBones.size(); j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, ikChainBones[j]) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBones[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}
	int size = ikChainBonesChildren.size();

	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}
	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	for (int i = 0; i < ikChainBonesChildren.size(); i++)
	{
		ikChainBonesFinal.push_back(ikChainBonesChildren[i]);
	}

	for (int i = 0; i < ikChainBones.size(); i++)
	{
		ikChainBonesFinal.push_back(ikChainBones[i]);
	}

	return ikChainBonesFinal;
}

std::vector<int> MDL::v49Mdl::v53GetAnimHdrBytesAdded(bool zeroFirst)
{
	std::vector<int> animBytesAdded;
	int bytesAdded = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int next = i + 1;
		animBytesAdded.push_back(bytesAdded);
		//Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
		if (animdescs[i].sectionindex == 0)
		{
			for (int j = lastHdr; j < anims.size(); j++)
			{
				int headerSize = GetAnimHeaderSize((int)anims[j].flags);
				lastHdr++;
				bytesAdded += (32 - headerSize);
				if (anims[j].nextoffset == 0)
				{

					if (animdescs[i].numikrules > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].ikruleindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim && animdescs[next].sectionindex == 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].sectionindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
			}
		}
		else animBytesAdded[i] = bytesAdded;
	}
	return animBytesAdded;
}

int MDL::v49Mdl::v53GetTotalAnimHdrBytesAdded()
{
	int bytesAdded = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int next = i + 1;

		if (animdescs[i].sectionindex > 0)
		{
			int frames = animdescs[i].numframes;
			int secFrames = animdescs[i].sectionframes;
			int num = (frames / secFrames) + 2;

			bytesAdded += -4 * num;

		}

		if (animdescs[i].sectionindex == 0)
		{
			for (int j = lastHdr; j < anims.size(); j++)
			{
				int headerSize = GetAnimHeaderSize((int)anims[j].flags);
				lastHdr++;
				bytesAdded += (32 - headerSize);
				if (anims[j].nextoffset == 0)
				{

					if (animdescs[i].numikrules > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].ikruleindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						bytesAdded += 32 - dist;
					}

					if (i + 1 > mdlhdr.numlocalanim - 1)
					{
						int dist = (mdlhdr.localseqindex - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("secDist:  %d\n", dist);
						//Logger::Info("secFinalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim && animdescs[next].sectionindex == 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].sectionindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
			}
		}
	}
	//Logger::Info("Total BytesAdded: %d\n", bytesAdded);
	return bytesAdded;
}

int MDL::v49Mdl::v53GetAnimHdrBytesAddedIdv(int anim)
{
	int bytesAdded = 0;
	int lastHdr = 0;

	for (int i = 0; i < anim + 1; i++)
	{
		int next = i + 1;
		if (animdescs[i].sectionindex == 0)
		{
			bytesAdded = 0;
			for (int j = lastHdr; j < anims.size(); j++)
			{
				int headerSize = GetAnimHeaderSize((int)anims[j].flags);
				lastHdr++;
				bytesAdded += (32 - headerSize);
				if (anims[j].nextoffset == 0)
				{

					if (animdescs[i].numikrules > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].ikruleindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						bytesAdded += 32 - dist;
					}

					if (i + 1 > mdlhdr.numlocalanim - 1)
					{
						int dist = (mdlhdr.localseqindex - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("secDist:  %d\n", dist);
						//Logger::Info("secFinalDist:  %d\n", 32 - dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex == 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].sectionindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
			}
		}
	}
	return bytesAdded;
}

std::vector<int> MDL::v49Mdl::v53GetSecHdrBytesAdded(bool zeroFirst)
{
	int bytesAdded = 0;
	std::vector<int> bytesAddedPerAnim;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		bytesAddedPerAnim.push_back(bytesAdded);
		//Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			int secDist = startPos + animdescs[i].animindex - (startPos + animdescs[i].sectionindex + (8 * num));

			bytesAdded -= secDist;

			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{

						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 > mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = (mdlhdr.localseqindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));
							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}
						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 < num - 1)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));
							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}
						break;
					}
				}
				secNum++;
			}
		}
	}
	return bytesAddedPerAnim;
}

int MDL::v49Mdl::v53GetTotalSecHdrBytesAdded()
{
	int bytesAdded = 0;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			int secDist = startPos + animdescs[i].animindex - (startPos + animdescs[i].sectionindex + (8 * num));

			bytesAdded -= secDist;

			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}

						if (i + 1 > mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = (mdlhdr.localseqindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}

						if (j + 1 < num - 1)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}
						break;
					}
				}
				secNum++;
			}
		}
	}
	//Logger::Info("Total BytesAdded: %d\n", bytesAdded);
	return bytesAdded;
}

int MDL::v49Mdl::v53GetSecHdrBytesAddedIdv(int anim)
{
	int bytesAdded = 0;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < anim + 1; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		//bytesAddedPerAnim.push_back(bytesAdded);
		//Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			int secDist = startPos + animdescs[i].animindex - (startPos + animdescs[i].sectionindex + (8 * num));

			bytesAdded -= secDist;
			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{

						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 > mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = (mdlhdr.localseqindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (j + 1 < num - 1)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}
						break;
					}
				}
				secNum++;
			}
		}
	}
	return bytesAdded;
}

std::vector<int> MDL::v49Mdl::v53GetSecBytesAdded(bool zeroFirst)
{
	int bytesAdded = 0;
	std::vector<int> bytesAddedPerSec;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			int secDist = startPos + animdescs[i].animindex - (startPos + animdescs[i].sectionindex + (8 * num));

			bytesAdded -= secDist;

			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				bytesAddedPerSec.push_back(bytesAdded);
				//Logger::Info("secBytesAdded: %d, Anim: %d\n", bytesAdded, i);
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (j + 1 < num - 1)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						break;
					}
				}
				secNum++;
			}
		}
	}
	return bytesAddedPerSec;
}

std::vector<int> MDL::v49Mdl::v53IkRuleStairsPerAnim()
{
	std::vector<int> ikRuleStairsPerAnim;
	int ikRuleNum = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		mstudioanimdesc_t_v49 v49AnimDesc = animdescs[i];
		int ikStairs = -12 * (ikRuleNum);

		ikRuleStairsPerAnim.push_back(ikStairs);
		if (v49AnimDesc.numikrules > 0)
		{
			ikRuleNum += v49AnimDesc.numikrules;
		}

	}
	return ikRuleStairsPerAnim;
}

std::vector<int> MDL::v49Mdl::v53GetBytesAddedToWeightLists()
{
	std::vector<int> hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	std::vector<int> secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	std::vector<int> bytesAddedToWeightLists;
	int ikRuleNum = 0;
	int seqIdx = mdlhdr.localseqindex;
	//Logger::Info("animIdx: %d\n", seqIdx);
	//Logger::Info("Hdr BytesAdded: %d\n", bytesAddedToHeader);
	//Logger::Info("Bone BytesAdded: %d\n", bytesAddedToBones);
	int bytesAddedToHeader = 52;
	int bytesAddedToBones = mdlhdr.numbones * 28;
	int actModNumber = 0;
	int bytesAdded = 0;

	for (int i = 0; i < mdlhdr.numlocalseq; i++)
	{
		//Logger::Info("Test\n");
		mstudioseqdescv49_t v49SeqDesc = seqdescs[i];
		int strPos = mdlhdr.localseqindex + 212 * i;

		if (!contains(weightlistidxs, strPos + seqdescs[i].weightlistindex))
		{
			weightlistidxs.push_back(strPos + seqdescs[i].weightlistindex);
			bytesAdded = (4 * actModNumber);
			bytesAddedToWeightLists.push_back(bytesAdded);
		}
//		Logger::Info("Bytes Added: %d \n", bytesAdded);
		actModNumber += v49SeqDesc.numactivitymodifiers;
	}
	return bytesAddedToWeightLists;
}

void MDL::v49Mdl::SetMdlInts()
{
	ikRuleStairsPerAnim = v53IkRuleStairsPerAnim();
	hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	secHdrBytesSecAdd = v53GetSecBytesAdded(true);

	animByteAddedTotal = v53GetTotalAnimHdrBytesAdded();
	animSecByteAddedTotal = v53GetTotalSecHdrBytesAdded();
	bytesAddedToWeightLists = v53GetBytesAddedToWeightLists();

	animBytesAdded = animByteAddedTotal + animSecByteAddedTotal;
	bytesAddedToRuiMesh = 0;
	bytesAddedToIkRules = -12 * ikrules.size();
	bytesAddedToHeader = 52;
	bytesAddedToBones = mdlhdr.numbones * 28;
	bytesAddedToAnims = -8 * mdlhdr.numlocalanim;
	bytesAddedToAnimData = mdlhdr.numlocalanim > 0 ? animByteAddedTotal + animSecByteAddedTotal + bytesAddedToIkRules + bytesAddedToSections : 0;
	bytesAddedToSections = -4 * secHdrBytesSecAdd.size();
	bytesAddedToSeqs = 20 * mdlhdr.numlocalseq;
	bytesAddedToTextures = -20 * mdlhdr.numtextures;
	bytesAddedToIkChains = 16 * mdlhdr.numikchains;
	bytesAddedToActMods = 4 * activitymodifiers.size();
	bytesAddedToLinearBone = -12 * mdlhdr.numbones;
	bytesAddedToAABBTree = 60;
	textureFiller = 0;
	strFiller = 60;
	allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh;
	numOfLinks = 0;
}

studiohdr_t_v53 MDL::v49Mdl::ConvertHeader(FileInfo info)
{

	int ruiNum = 0;
	int mdlSize = 0;
	int phySize = 0;
	int phyBSize = 0;
	int vtxSize = 0;
	int vvdSize = 0;
	int vvcSize = 0;
	int ruiSize = 0;
	int aabbSize = 0;

	if (info.rui.has_value()) //This is a temp for rui testing. -Liberty
	{
		BinaryReader RUIStream = BinaryReader(info.rui.value().c_str());
		bytesAddedToRuiMesh = RUIStream.size;
		ruiSize = RUIStream.size;
		bytesAddedToRuiMesh = ruiSize;
		mstudiorruiheader_t ruiHeader;
		mstudioruimesh_t ruiMesh;
		int meshIndex = 0;
		RUIStream.Read(&ruiHeader);
		RUIStream.seek(ruiHeader.ruimeshindex);
//		Logger::Info("Index:  %d\n", RUIStream.size);

		for (int i = 0; i < 64; i++)
		{
			//RUIStream.Read(&ruiHeader);
			//if (i == 0)meshIndex = ruiHeader.ruimeshindex;
			//if (i == 0)RUIStream.seek(ruiHeader.ruimeshindex);
			RUIStream.Read(&ruiMesh);
			ruiNum++;
//			Logger::Info("Pos:  %d Num: %d\n", RUIStream.Position(), i);


			if (RUIStream.Position() >= RUIStream.size)
			{
				break;
			}
		}
		RUIStream.seek(0);
		RUIStream.Read(&ruiHeader);
		int ruiPos = mdlhdr.includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + 8 * mdlhdr.numincludemodels;
		int dist = ruiHeader.ruimeshindex - 8 * ruiNum;
//		Logger::Info("Index:  %d\n", ruiHeader.ruimeshindex);
		double ruiAlignment = (ruiPos + 8 * ruiNum) / (double)16;
//		Logger::Info("Alignment:  %f\n", ruiAlignment);
		if (!IsWhole(ruiAlignment))
		{
			int ruiAlignment2 = 16 * ((int)ruiAlignment + 1);
			int paddingFix = ruiAlignment2 - ruiPos;
//			Logger::Info("Padding:  %d\n", paddingFix - 8 * ruiNum);
//			Logger::Info("Padding2:  %d\n", ruiAlignment2);
//			Logger::Info("Padding3:  %d\n", ruiPos);
			ruiPadding = paddingFix - 8 * ruiNum;
			ruiSize += paddingFix - 8 * ruiNum;
			bytesAddedToRuiMesh = ruiSize;
		}
	}


	if (info.mdl.has_value())
	{
		BinaryReader MdlStream = BinaryReader(info.mdl.value().c_str());
		mdlSize = MdlStream.size;
	}
	if (info.phy.has_value())
	{
		BinaryReader PhyStream = BinaryReader(info.phy.value().c_str());
		phySize = PhyStream.size;
	}
	if (info.pfb.has_value())
	{
		BinaryReader PhyBStream = BinaryReader(info.pfb.value().c_str());
		phyBSize = PhyBStream.size;
	}
	if (info.vtx.has_value())
	{
		BinaryReader VtxStream = BinaryReader(info.vtx.value().c_str());
		vtxSize = VtxStream.size;
	}
	if (info.vvd.has_value())
	{
		BinaryReader VvdStream = BinaryReader(info.vvd.value().c_str());
		vvdSize = VvdStream.size;
	}

	if (info.vvc.has_value())
	{
		BinaryReader VvcStream = BinaryReader(info.vvc.value().c_str());
		vvcSize = VvcStream.size;
	}

	if (info.aabb.has_value())
	{
		BinaryReader AABBStream = BinaryReader(info.aabb.value().c_str());
		aabbSize = AABBStream.size;
		bytesAddedToAABBTree = aabbSize;
	}
	else
		bytesAddedToAABBTree = 60;

	Logger::Notice("RuiSize %d\n", ruiSize);

	int			id;
	int			version;
	int			checksum;
	int			sznameindex = mdlsubhdr.sznameindex + allBytesAdded + 408 + ruiSize + bytesAddedToLinearBone + bytesAddedToAABBTree;
	char		name[64];

	int			length;

	Vector3		eyeposition;

	Vector3		illumposition;

	Vector3		hull_min;
	Vector3		hull_max;

	Vector3		view_bbmin;
	Vector3		view_bbmax;

	mdlflags_t	flags;

	int			numbones;
	int			boneindex = mdlhdr.boneindex + bytesAddedToHeader;

	int			numbonecontrollers;
	int			bonecontrollerindex = mdlhdr.bonecontrollerindex + bytesAddedToHeader + bytesAddedToBones;

	int			numhitboxsets;
	int			hitboxsetindex = mdlhdr.hitboxsetindex + bytesAddedToHeader + bytesAddedToBones;

	int			numlocalanim;
	int			localanimindex = mdlhdr.localanimindex + bytesAddedToHeader + bytesAddedToBones;

	int			numlocalseq;
	int			localseqindex = mdlhdr.localseqindex + bytesAddedToAnims + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;

	int			activitylistversion;
	int			eventsindexed;

	int			numtextures;
	int			textureindex = mdlhdr.textureindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + ruiSize;

	int			numcdtextures;
	int			cdtextureindex = mdlhdr.cdtextureindex + allBytesAdded + ruiSize;

	int			numskinref;
	int			numskinfamilies;
	int			skinindex = mdlhdr.skinindex + allBytesAdded + ruiSize;

	int			numbodyparts;
	int			bodypartindex = mdlhdr.bodypartindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

	int			numlocalattachments;
	int			localattachmentindex = mdlhdr.localattachmentindex + bytesAddedToHeader + bytesAddedToBones;

	int			numlocalnodes;
	int			localnodeindex = mdlhdr.localnodeindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;
	int			localnodenameindex = mdlhdr.localnodenameindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

	int			numflexdesc;
	int			flexdescindex = mdlhdr.flexdescindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			numflexcontrollers;
	int			flexcontrollerindex = mdlhdr.flexcontrollerindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			numflexrules;
	int			flexruleindex = mdlhdr.flexruleindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			numikchains;
	int			ikchainindex = mdlhdr.ikchainindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

	int			nummouths = ruiNum;
	int			mouthindex = mdlhdr.includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + 8 * mdlhdr.numincludemodels;

	int			numlocalposeparameters;
	int			localposeparamindex = mdlhdr.localposeparamindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			surfacepropindex = mdlhdr.surfacepropindex + allBytesAdded + ruiSize + bytesAddedToLinearBone + bytesAddedToAABBTree;

	int			keyvalueindex = mdlhdr.keyvalueindex + allBytesAdded + ruiSize;
	int			keyvaluesize;

	int			numlocalikautoplaylocks;
	int			localikautoplaylockindex = mdlhdr.localikautoplaylockindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;


	float		mass;
	int			contents;

	int			numincludemodels;
	int			includemodelindex = mdlhdr.includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	uint32_t	virtualModel;

	int			bonetablebynameindex = mdlhdr.bonetablebynameindex + bytesAddedToHeader + bytesAddedToBones;

	std::byte	constdirectionallightdot;

	std::byte	rootLOD;

	std::byte	numAllowedRootLODs;

	std::byte	unused;

	float		fadedistance;

	int			numflexcontrollerui;
	int			flexcontrolleruiindex = mdlhdr.flexcontrolleruiindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	uint32_t	pVertexBase;
	uint32_t	pIndexBase;

	int			mayaindex = 0;

	int			numsrcbonetransform;
	int			srcbonetransformindex = mdlsubhdr.srcbonetransformindex + allBytesAdded + ruiSize;

	int			illumpositionattachmentindex;

	int			linearboneindex = linearboneindex > 0 || mdlhdr.numbones > 1 ? mdlsubhdr.linearboneindex + allBytesAdded + 408 + ruiSize : 0;

	int			m_nBoneFlexDriverCount;
	int			m_nBoneFlexDriverIndex;

	int			aabbindex = mdlsubhdr.sznameindex + allBytesAdded + 408 - 61 + ruiSize + bytesAddedToLinearBone + bytesAddedToAABBTree;
	int			numaabb = 0;
	int			numaabb1 = 0;
	int			numaabb2 = 0;

	int			unkstringindex = mdlsubhdr.sznameindex + allBytesAdded + ruiSize + bytesAddedToLinearBone + bytesAddedToAABBTree;

	int			vtxindex = vtxSize > 0 ? mdlSize + allBytesAdded + phySize + phyBSize + ruiSize + bytesAddedToLinearBone + bytesAddedToAABBTree : 0; // VTX
	int			vvdindex = vvdSize > 0 ? mdlSize + allBytesAdded + phySize + vtxSize + phyBSize + ruiSize + bytesAddedToLinearBone + bytesAddedToAABBTree : 0; // VVD / IDSV
	int			vvcindex = vvcSize > 0 ? mdlSize + allBytesAdded + phySize + vtxSize + phyBSize + ruiSize + vvdSize + bytesAddedToLinearBone + bytesAddedToAABBTree : 0; // VVC / IDCV 
	int			vphyindex = phySize > 0 ? mdlSize + allBytesAdded + ruiSize + bytesAddedToLinearBone + bytesAddedToAABBTree : 0; // VPHY / IVPS

	int			vtxsize = vtxSize; // VTX
	int			vvdsize = vvdSize; // VVD / IDSV
	int			vvcsize = vvcSize; // VVC / IDCV 
	int			vphysize = phySize; // VPHY / IVPS

	int			unkmemberindex1 = mdlSize + allBytesAdded + phySize + ruiSize + bytesAddedToLinearBone + bytesAddedToAABBTree;
	int			numunkmember1 = phyBSize > 0 ? 0 : 0;

	int			unk = 0;

	int			unkindex3 = mdlSize + allBytesAdded + phySize + ruiSize + bytesAddedToLinearBone + bytesAddedToAABBTree;

	int			unused1[60];

	studiohdr_t_v53 v53Hdr = { mdlhdr.id, 53, mdlhdr.checksum, sznameindex, ArrayToVector(mdlhdr.name, 64), mdlhdr.length, mdlhdr.eyeposition, mdlhdr.illumposition, mdlhdr.hull_min, mdlhdr.hull_max, mdlhdr.view_bbmin, mdlhdr.view_bbmax, mdlhdr.flags, mdlhdr.numbones, boneindex, mdlhdr.numbonecontrollers, bonecontrollerindex, mdlhdr.numhitboxsets, hitboxsetindex, mdlhdr.numlocalanim, localanimindex, mdlhdr.numlocalseq, localseqindex, mdlhdr.activitylistversion, mdlhdr.eventsindexed, mdlhdr.numtextures, textureindex, mdlhdr.numcdtextures, cdtextureindex, mdlhdr.numskinref, mdlhdr.numskinfamilies, skinindex, mdlhdr.numbodyparts, bodypartindex, mdlhdr.numlocalattachments, localattachmentindex, mdlhdr.numlocalnodes, localnodeindex, localnodenameindex, mdlhdr.numflexdesc, flexdescindex, mdlhdr.numflexcontrollers, flexcontrollerindex, mdlhdr.numflexrules, flexruleindex, mdlhdr.numikchains, ikchainindex, ruiNum, mouthindex, mdlhdr.numlocalposeparameters, localposeparamindex, surfacepropindex, keyvalueindex, mdlhdr.keyvaluesize, mdlhdr.numlocalikautoplaylocks, localikautoplaylockindex, mdlhdr.mass, mdlhdr.contents, mdlhdr.numincludemodels, includemodelindex, mdlhdr.virtualModel, bonetablebynameindex, mdlhdr.constdirectionallightdot, mdlhdr.rootLOD, mdlhdr.numAllowedRootLODs, mdlhdr.unused, -1.0, mdlhdr.numflexcontrollerui, flexcontrolleruiindex, mdlhdr.pVertexBase, mdlhdr.pIndexBase, mayaindex, mdlsubhdr.numsrcbonetransform, srcbonetransformindex, mdlsubhdr.illumpositionattachmentindex, linearboneindex, mdlsubhdr.m_nBoneFlexDriverCount, mdlsubhdr.m_nBoneFlexDriverIndex, aabbindex, numaabb, numaabb1, numaabb2, unkstringindex, vtxindex, vvdindex, vvcindex, vphyindex, vtxsize, vvdsize, vvcsize, vphysize, unkmemberindex1, numunkmember1, unk, unkindex3, unused1 };

	return v53Hdr;
}

std::vector<mstudiobone_t_v53> MDL::v49Mdl::BoneConversion()
{
	std::vector<mstudiobone_t_v53> v53Bones;
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		int stairs = bytesAddedToAnims + bytesAddedToSeqs + (28 * mdlhdr.numbones) - (28 * i) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;

		mstudiobone_t_v49 v49Bone = bones[i];

		Vector3 unkvector{ 1,1,1 };

		Vector3 unkvector1{ 0.0004882813 ,0.0004882813 ,0.0004882813 };

		int unkindex = 0;

		Vector3 posscale{ 0,0,0 };

		if (v49Bone.procindex > 0) v49Bone.procindex += ( (28 * mdlhdr.numbones) - (28 * i) );
		if (v49Bone.surfacepropidx > 0) v49Bone.surfacepropidx += stairs;
		if (v49Bone.surfacepropLookup > 0) v49Bone.surfacepropLookup += stairs;

		mstudiobone_t_v53 bone{ v49Bone.sznameindex + stairs, v49Bone.parent, -1,-1,-1,-1,-1,-1, v49Bone.pos, v49Bone.quat, v49Bone.rot, unkvector, posscale, v49Bone.rotscale, unkvector1, v49Bone.poseToBone, v49Bone.qAlignment, v49Bone.flags, v49Bone.proctype, v49Bone.procindex, v49Bone.physicsbone, v49Bone.surfacepropidx, v49Bone.contents, v49Bone.surfacepropLookup, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		v53Bones.push_back(bone);
	}
	return v53Bones;
}

std::vector<mstudiobbox_t_v53> MDL::v49Mdl::HitboxConversion()
{
	std::vector<mstudiobbox_t_v53> v53Hitboxes;
	int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;
	for (int i = 0; i < hitboxes.size(); i++)
	{
		mstudiobbox_t_v49 v49Hitbox = hitboxes[i];

		mstudiobbox_t_v53 hitbox{ v49Hitbox.bone, v49Hitbox.group, v49Hitbox.bbmin, v49Hitbox.bbmax, v49Hitbox.szhitboxnameindex + stairs, 0, v49Hitbox.szhitboxnameindex + stairs, 0,0,0,0,0,0 };

		v53Hitboxes.push_back(hitbox);
	}

	return v53Hitboxes;
}

std::vector<mstudioanimdesc_t_v53> MDL::v49Mdl::AnimDescConversion()
{
	std::vector<int> hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	std::vector<int> secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	std::vector<mstudioanimdesc_t_v53> v53AnimDescs;
	int ikRuleNum = 0;
	int animIdx = mdlhdr.localanimindex;
	//Logger::Info("animIdx: %d\n", animIdx);
	//Logger::Info("Hdr BytesAdded: %d\n", bytesAddedToHeader);
	//Logger::Info("Bone BytesAdded: %d\n", bytesAddedToBones);
	int bytesAddedToHeader = 52;
	int bytesAddedToBones = mdlhdr.numbones * 28;
	int secNum = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		//Logger::Info("Test\n");
		mstudioanimdesc_t_v49 v49AnimDesc = animdescs[i];

		int frames = v49AnimDesc.numframes;
		int secFrames = v49AnimDesc.sectionframes;
		int outPos = animIdx + bytesAddedToHeader + bytesAddedToBones + 92 * i;
		int pos = animIdx + 100 * i;
		int stairs = (-((8 * (mdlhdr.numlocalanim - i))) + bytesAddedToSeqs) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;
		int steps = -((8 * (mdlhdr.numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i];

		int ikStairs = -12 * (ikRuleNum);
		if (v49AnimDesc.numikrules > 0)
		{
			int stairs = v53GetAnimHdrBytesAddedIdv(i);
			int stairs2 = 0;
			if (v49AnimDesc.sectionindex > 0) stairs2 = v53GetSecHdrBytesAddedIdv(i);
			v49AnimDesc.ikruleindex += stairs + stairs2;
			ikRuleNum += v49AnimDesc.numikrules;
		}
		int secStairs = -4 * secNum;
		if (v49AnimDesc.sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;
			secNum += num;
			int dist = (((pos + animdescs[i].sectionindex) + (8 * num)) - (pos + animdescs[i].animindex)) * -1;
		}

		if (v49AnimDesc.animindex > 0) v49AnimDesc.animindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.movementindex > 0) v49AnimDesc.movementindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.ikruleindex > 0) v49AnimDesc.ikruleindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.localhierarchyindex > 0) v49AnimDesc.localhierarchyindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.zeroframeindex > 0) v49AnimDesc.zeroframeindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.ikrulezeroframeindex > 0) v49AnimDesc.ikrulezeroframeindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.sectionindex > 0) v49AnimDesc.sectionindex += steps + ikStairs + secStairs;

		int compressedIkRuleIdx = 0; //if (v49AnimDesc.numikrules > 0) compressedIkRuleIdx = ( basePtr - ( basePtr - (v49AnimDesc.ikruleindex + 140 * v49AnimDesc.numikrules) ) ) * -1 ;


		mstudioanimdesc_t_v53 animDesc = { -outPos, v49AnimDesc.sznameindex + stairs, v49AnimDesc.fps, v49AnimDesc.flags, v49AnimDesc.numframes, v49AnimDesc.nummovements, v49AnimDesc.movementindex, compressedIkRuleIdx * -1, v49AnimDesc.animindex, v49AnimDesc.numikrules, v49AnimDesc.ikruleindex, v49AnimDesc.numlocalhierarchy, v49AnimDesc.localhierarchyindex, v49AnimDesc.sectionindex, v49AnimDesc.sectionframes, v49AnimDesc.zeroframespan, v49AnimDesc.zeroframecount, v49AnimDesc.zeroframeindex, v49AnimDesc.zeroframestalltime, 0, 0, 0, 0, 0 };

		v53AnimDescs.push_back(animDesc);
	}
	return v53AnimDescs;
}

std::vector<mstudioseqdesc_t_v53> MDL::v49Mdl::SeqDescConversion()
{
	std::vector<int> hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	std::vector<int> secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	std::vector<mstudioseqdesc_t_v53> v53SeqDescs;
	int ikRuleNum = 0;
	int seqIdx = mdlhdr.localseqindex;
	//Logger::Info("animIdx: %d\n", seqIdx);
	//Logger::Info("Hdr BytesAdded: %d\n", bytesAddedToHeader);
	//Logger::Info("Bone BytesAdded: %d\n", bytesAddedToBones);
	int bytesAddedToHeader = 52;
	int bytesAddedToBones = mdlhdr.numbones * 28;
	int actModNumber = 0;

	for (int i = 0; i < mdlhdr.numlocalseq; i++)
	{
		//Logger::Info("Test\n");
		mstudioseqdescv49_t v49SeqDesc = seqdescs[i];
		int strPos = mdlhdr.localseqindex + 212 * i;
		int weightList = GetTrgtPosInArry(weightlistidxs, strPos + seqdescs[i].weightlistindex);
		int steps = 20 * (mdlhdr.numlocalseq - i);
		int steps2 = steps + 4 * actModNumber;
		int stairs1 = steps + bytesAddedToWeightLists[weightList];//strPos + v49SeqDesc.weightlistindex == weightlistidxs[0] ? 20 * (mdlhdr.numlocalseq - i) : 20 * (mdlhdr.numlocalseq - i) + 4 * actModNumber;
		int stairs = steps + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;

		int outPos = seqIdx + stairs;

		for (int j = 0; j < v49SeqDesc.numblends; j++)
		{
			int anim = v49SeqDesc.blends.blends[j];
			if (animdescs[anim].numframes == 1) v49SeqDesc.flags += 0x80000;
		}

		if (v49SeqDesc.eventindex > 0)				v49SeqDesc.eventindex += steps2;
		if (v49SeqDesc.animindexindex > 0)			v49SeqDesc.animindexindex += steps2;
		if (v49SeqDesc.movementindex > 0)			v49SeqDesc.movementindex += steps2;
		if (v49SeqDesc.autolayerindex > 0)			v49SeqDesc.autolayerindex += steps2;
		if (v49SeqDesc.weightlistindex > 0)			v49SeqDesc.weightlistindex += stairs1;
		if (v49SeqDesc.posekeyindex > 0)			v49SeqDesc.posekeyindex += steps2;
		if (v49SeqDesc.iklockindex > 0)				v49SeqDesc.iklockindex += steps2;
		if (v49SeqDesc.keyvalueindex > 0)			v49SeqDesc.keyvalueindex += steps2;
		if (v49SeqDesc.cycleposeindex > 0)			v49SeqDesc.cycleposeindex += steps2;
		if (v49SeqDesc.activitymodifierindex > 0)	v49SeqDesc.activitymodifierindex += steps2;

		int compressedIkRuleIdx = 0; //if (v49AnimDesc.numikrules > 0) compressedIkRuleIdx = ( basePtr - ( basePtr - (v49AnimDesc.ikruleindex + 140 * v49AnimDesc.numikrules) ) ) * -1 ;


		mstudioseqdesc_t_v53 v53SeqDesc{ -outPos, v49SeqDesc.szlabelindex + stairs, v49SeqDesc.szactivitynameindex + stairs, v49SeqDesc.flags, v49SeqDesc.activity, v49SeqDesc.actweight, v49SeqDesc.numevents, v49SeqDesc.eventindex, v49SeqDesc.bbmin, v49SeqDesc.bbmax, v49SeqDesc.numblends, v49SeqDesc.animindexindex, v49SeqDesc.movementindex, v49SeqDesc.groupsize[0], v49SeqDesc.groupsize[1], v49SeqDesc.paramindex[0], v49SeqDesc.paramindex[1], v49SeqDesc.paramstart[0], v49SeqDesc.paramstart[1], v49SeqDesc.paramend[0] , v49SeqDesc.paramend[1], v49SeqDesc.paramparent, v49SeqDesc.fadeintime, v49SeqDesc.fadeouttime, v49SeqDesc.localentrynode, v49SeqDesc.localexitnode, v49SeqDesc.nodeflags, v49SeqDesc.entryphase, v49SeqDesc.exitphase, v49SeqDesc.lastframe, v49SeqDesc.nextseq, v49SeqDesc.pose, v49SeqDesc.numikrules, v49SeqDesc.numautolayers, v49SeqDesc.autolayerindex, v49SeqDesc.weightlistindex, v49SeqDesc.posekeyindex, v49SeqDesc.numiklocks, v49SeqDesc.iklockindex, v49SeqDesc.keyvalueindex, v49SeqDesc.keyvaluesize, v49SeqDesc.cycleposeindex, v49SeqDesc.activitymodifierindex, v49SeqDesc.numactivitymodifiers, 0,0,0,0,0,0,0,0,0,0 };

		v53SeqDescs.push_back(v53SeqDesc);
		actModNumber += v49SeqDesc.numactivitymodifiers;
	}
	return v53SeqDescs;
}

std::vector<mstudioactivitymodifier_t_v53> MDL::v49Mdl::ActivityModifierConversion()
{
	std::vector<mstudioactivitymodifier_t_v53>	v53ActivityModifiers;
	std::vector<std::string> sznames;
	int actModNum = 0;
	for (int i = 0; i < seqdescs.size(); i++)
	{
		//Logger::Info("ActName: %s\n", stringtable.seqs[i].activity.c_str());
		//for (int j = 0; j < seqdescs[i].numactivitymodifiers; j++)
		//{
		//	//Logger::Info("ActModNameSize: %d\n", 0);
		//	Logger::Info("ActModName: %s\n", stringtable.seqs[i].activitymodifier[j].c_str());
		//	actModNum++;
		//}
		std::string t1;
		std::string t2;
		if (seqdescs[i].numactivitymodifiers > 0)
		{
			for (int j = 0; j < seqdescs[i].numactivitymodifiers; j++)
			{
				int prevActMod = actModNum - 1;
				int nextActMod = actModNum + 1;
				bool isTransgender = seqdescs[i].numactivitymodifiers == 2;
				bool shouldTransition = false;
				std::pair<std::string, std::string> actModPair;

				int unk = 0;

				if (isTransgender)
				{
					if (j == 0)
					{
						activitymodifiers[actModNum].szname.resize(activitymodifiers[actModNum].szname.size() - 1);
						t1 = activitymodifiers[actModNum].szname;
					}
					if (j == 1)
					{
						activitymodifiers[actModNum].szname.resize(activitymodifiers[actModNum].szname.size() - 1);
						t2 = activitymodifiers[actModNum].szname;
					}

					if (j == 1)
					{
						actModPair = { t1, t2 };
						unk = 1;

						if (actModPair.first == "sliding" && actModPair.second == "air" || actModPair.first == "air" && actModPair.second == "sliding")
						{
							unk = 0;
						}
					}
				}

				int stairs = 4 * (activitymodifiers.size() - actModNum) + bytesAddedToRuiMesh + bytesAddedToTextures + bytesAddedToIkChains + textureFiller + bytesAddedToLinearBone + bytesAddedToAABBTree;
				if (activitymodifiers[actModNum].sznameindex > 0) activitymodifiers[actModNum].sznameindex += stairs;

				//if(isTransgender)Logger::Info("ActModConv Read: %s\n", activitymodifiers[actModNum].szname.c_str());

				mstudioactivitymodifier_t_v53 activityModifier = { activitymodifiers[actModNum].sznameindex, unk, activitymodifiers[actModNum].szname };
				v53ActivityModifiers.push_back(activityModifier);
				actModNum++;
			}
		}

	}
	//for (int i = 0; i < activitymodifiers.size(); i++)
	//{
	//	int unk = 0;
	//	int stairs = 4 * (activitymodifiers.size() - i) + bytesAddedToRuiMesh + bytesAddedToTextures + bytesAddedToIkChains + textureFiller + strFiller;
	//	if (activitymodifiers[i].sznameindex > 0) activitymodifiers[i].sznameindex += stairs;
	//
	//	mstudioactivitymodifier_t_v53 activityModifier = { activitymodifiers[i].sznameindex, unk, activitymodifiers[i].szname };
	//	v53ActivityModifiers.push_back(activityModifier);
	//}
	return v53ActivityModifiers;
}

std::vector<mstudioikrule_t_v53> MDL::v49Mdl::IkRuleConversion()
{
	std::vector<mstudioikrule_t_v53> v53IkRules;
	int numIkRules = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		mstudioanimdesc_t_v49 v49AnimDesc = animdescs[i];
		if (v49AnimDesc.numikrules > 0)
		{
			for (int j = 0; j < v49AnimDesc.numikrules; j++)
			{
				int ikStairs = -12 * (v49AnimDesc.numikrules - j);

				mstudioikrule_t_v49 v49IkRule = ikrules[numIkRules];
				numIkRules++;

				if (v49IkRule.compressedikerrorindex > 0) v49IkRule.compressedikerrorindex += ikStairs;
				if (v49IkRule.ikerrorindex > 0) v49IkRule.ikerrorindex += ikStairs;
				if (v49IkRule.szattachmentindex > 0) v49IkRule.szattachmentindex += ikStairs;

				mstudioikrule_t_v53 ikRule = { v49IkRule.index, ikruletype(v49IkRule.type), v49IkRule.chain, v49IkRule.bone, v49IkRule.slot, v49IkRule.height, v49IkRule.radius, v49IkRule.floor, v49IkRule.pos, v49IkRule.q.one, v49IkRule.q.i, v49IkRule.q.j, v49IkRule.q.k, v49IkRule.compressedikerrorindex, v49IkRule.iStart, v49IkRule.ikerrorindex, v49IkRule.start, v49IkRule.peak, v49IkRule.tail, v49IkRule.end, v49IkRule.contact, v49IkRule.drop, v49IkRule.top, v49IkRule.szattachmentindex, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

				v53IkRules.push_back(ikRule);
			}
		}
	}
	return v53IkRules;
}

std::vector<mstudioanim_t_v53> MDL::v49Mdl::ConvertAnims()
{
	std::vector<mstudioanim_t_v49> _anims = anims;
	std::vector<mstudioanim_t_v53> v53Anims;
	mstudiolinearbonedata_t_v49 lbd = linearbonedata;

	for (int i = 0; i < anims.size(); i++)
	{

		bool hasAnimPosPtr =	(int)anims[i].flags & STUDIO_ANIM_ANIMPOS;
		bool hasAnimRotPtr =	(int)anims[i].flags & STUDIO_ANIM_ANIMROT;
		bool hasRawPos =		(int)anims[i].flags & STUDIO_ANIM_RAWPOS;
		bool hasRawRot =		(int)anims[i].flags & STUDIO_ANIM_RAWROT;
		bool hasRawRot2 =		(int)anims[i].flags & STUDIO_ANIM_RAWROT2;


		float _posscale = 0; // does what posscale is used for

		std::byte _bone; // unsigned byte, bone limit exceeds 128 so has to be. also means max bones is 255.
		std::byte _flags;

		int16_t _unk = 0; // normally null data


		QuaternionShort _rawrot = { 0,0,0, 0 }; // pQuat64

		mstudioanim_valueptr_t_v49 _animrot = { 0,0,0 }; // pRotV
		int16_t _unused = 0; // pretty sure


		Vector3Short _rawpos = { 0,0,0 }; // pPos
		mstudioanim_valueptr_t_v49 _animpos = { 0,0,0 }; // pPosV

		Vector3Short _rawscale = { 15360, 15360, 15360 }; // new in v53
		mstudioanim_valueptr_t_v49 _animscale = { 0,0,0 };

		int _nextOffset = (int)anims[i].nextoffset > 0 ? (int)anims[i].nextoffset + (32 - GetAnimHeaderSize((int)anims[i].flags)) : 0;

		int bone = (int)anims[i].bone;
		_bone = anims[i].bone;
		_flags = (std::byte)ConvertFlag((int)anims[i].flags);

		if (hasAnimPosPtr && hasAnimRotPtr) _posscale = GetLargestNumber(lbd.posScale[bone].x, lbd.posScale[bone].y, lbd.posScale[bone].z);
		if ((int)anims[i].flags & STUDIO_ANIM_ANIMROT)
		{
			Vector3Short rotPtr = anims[i].animrot.offset;
			if (!hasAnimPosPtr)
			{
				rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
				anims[i].animrot.offset = rotPtr;
			}
			else
			{
				rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
				anims[i].animrot.offset = rotPtr;
			}
			_animrot = anims[i].animrot;

		}

		if ((int)anims[i].flags & STUDIO_ANIM_ANIMPOS)
		{
			Vector3Short posPtr = anims[i].animpos.offset;
			if (!hasAnimRotPtr)
			{
				posPtr.x > 0 ? posPtr.x += 0 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 0 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 0 : posPtr.z = 0;
				anims[i].animpos.offset = posPtr;
			}
			else
			{
				posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
				anims[i].animpos.offset = posPtr;
			}
			_animpos = anims[i].animpos;
		}

		if ((int)anims[i].flags & STUDIO_ANIM_RAWROT) _rawrot = anims[i].rawrot;

		if ((int)anims[i].flags & STUDIO_ANIM_RAWROT2) _rawrot = anims[i].rawrot2;

		if ((int)anims[i].flags & STUDIO_ANIM_RAWPOS) _rawpos = anims[i].rawpos;
		else
			_rawpos = { (short)float_to_half(lbd.bonePos[bone].x), (short)float_to_half(lbd.bonePos[bone].y), (short)float_to_half(lbd.bonePos[bone].z) };

		mstudioanim_t_v53 v53Anim{ _posscale, _bone, _flags, _unk, _rawrot, _animrot, _unused, _rawpos, _animpos, _rawscale, _animscale, _nextOffset, anims[i].animdata };
		v53Anims.push_back(v53Anim);
	}
	return v53Anims;
}

std::vector<sectionindexes_t_v53> MDL::v49Mdl::ConvertSectionIndexes()
{
	std::vector<sectionindexesindex_t_v49> _sectionIndexes = sectionindexes;
	std::vector<sectionindexes_t_v53> v53SectionIndexes;
	int secNum = 0;
	int secSetNum = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int pos = mdlhdr.localanimindex + 100 * i;
		int stairs = -((8 * (mdlhdr.numlocalanim - i))) + hdrBytesAnimDescAdd[i];

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;
			secSetNum += num;

			int dist = pos + animdescs[i].animindex - (pos + animdescs[i].sectionindex + (8 * num));
			//Logger::Info("DistStr: %d\n", pos + animdescs[i].sectionindex + (8 * num) );
			//Logger::Info("DistEnd: %d\n", pos + animdescs[i].animindex);
			//Logger::Info("Dist: %d\n", dist);

			for (int j = 0; j < num; j++)
			{
				int steps = -4 * secSetNum;
				int offset = sectionindexes[secNum].sectionoffset + stairs + secHdrBytesSecAdd[secNum] + steps;

				sectionindexes_t_v53 v53SectionIndex{ offset };
				v53SectionIndexes.push_back(v53SectionIndex);
				secNum++;
			}
		}
	}
	return v53SectionIndexes;
}

std::vector<mstudioanim_t_v53> MDL::v49Mdl::ConvertSections()
{
	std::vector<mstudioanim_t_v49> _sections = sections;
	std::vector<mstudioanim_t_v53> v53Sections;
	mstudiolinearbonedata_t_v49 lbd = linearbonedata;

	for (int i = 0; i < sections.size(); i++)
	{


		bool hasAnimPosPtr =	(int)sections[i].flags & STUDIO_ANIM_ANIMPOS;
		bool hasAnimRotPtr =	(int)sections[i].flags & STUDIO_ANIM_ANIMROT;
		bool hasRawPos =		(int)sections[i].flags & STUDIO_ANIM_RAWPOS;
		bool hasRawRot =		(int)sections[i].flags & STUDIO_ANIM_RAWROT;
		bool hasRawRot2 =		(int)sections[i].flags & STUDIO_ANIM_RAWROT2;

		float _posscale = 0; // does what posscale is used for

		std::byte _bone; // unsigned byte, bone limit exceeds 128 so has to be. also means max bones is 255.
		std::byte _flags;

		int16_t _unk = 0; // normally null data


		QuaternionShort _rawrot = { 0,0,0, 0 }; // pQuat64

		mstudioanim_valueptr_t_v49 _animrot = { 0,0,0 }; // pRotV
		int16_t _unused = 0; // pretty sure


		Vector3Short _rawpos = { 0,0,0 }; // pPos
		mstudioanim_valueptr_t_v49 _animpos = { 0,0,0 }; // pPosV

		Vector3Short _rawscale = { 15360, 15360, 15360 }; // new in v53
		mstudioanim_valueptr_t_v49 _animscale = { 0,0,0 };

		int _nextOffset = (int)sections[i].nextoffset > 0 ? (int)sections[i].nextoffset + (32 - GetAnimHeaderSize((int)sections[i].flags)) : 0;

		int bone = (int)sections[i].bone;
		_bone = sections[i].bone;
		_flags = (std::byte)ConvertFlag((int)sections[i].flags);

		if ((int)sections[i].flags == 12) _posscale = GetLargestNumber(lbd.posScale[bone].x, lbd.posScale[bone].y, lbd.posScale[bone].z);
		if ((int)sections[i].flags & STUDIO_ANIM_ANIMROT)
		{
			Vector3Short rotPtr = sections[i].animrot.offset;
			if (!hasAnimPosPtr)
			{
				rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
				sections[i].animrot.offset = rotPtr;
			}
			else
			{
				rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
				sections[i].animrot.offset = rotPtr;
			}
			_animrot = sections[i].animrot;

		}

		if ((int)sections[i].flags & STUDIO_ANIM_ANIMPOS)
		{
			Vector3Short posPtr = sections[i].animpos.offset;
			if (!hasAnimRotPtr)
			{
				posPtr.x > 0 ? posPtr.x += 18 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 18 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 18 : posPtr.z = 0;
				sections[i].animpos.offset = posPtr;
			}
			else
			{
				posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
				sections[i].animpos.offset = posPtr;
			}
			_animpos = sections[i].animpos;
		}

		if (hasRawRot) _rawrot = sections[i].rawrot;

		if (hasRawRot2) _rawrot = sections[i].rawrot2;

		if (hasRawPos) _rawpos = sections[i].rawpos;
		else
			_rawpos = { (short)float_to_half(lbd.bonePos[bone].x), (short)float_to_half(lbd.bonePos[bone].y), (short)float_to_half(lbd.bonePos[bone].z) };

		mstudioanim_t_v53 v53Anim{ _posscale, _bone, _flags, _unk, _rawrot, _animrot, _unused, _rawpos, _animpos, _rawscale, _animscale, _nextOffset, sections[i].animdata };
		v53Sections.push_back(v53Anim);
	}
	return v53Sections;
}

std::vector<mstudioikchain_t_v53> MDL::v49Mdl::IkChainConversion()
{
	std::vector<mstudioikchain_t_v53> v53IkChains;
	for (int i = 0; i < ikchains.size(); i++)
	{
		int stairs = 16 * (ikchains.size() - i);
		int offset = bytesAddedToTextures + textureFiller + strFiller + stairs + bytesAddedToRuiMesh + bytesAddedToLinearBone;
		float unk = (float)0.707;
		if (ikchains[i].sznameindex > 0) ikchains[i].sznameindex += offset;
		if (ikchains[i].linkindex > 0) ikchains[i].linkindex += stairs;

		mstudioikchain_t_v53 ikChain = { ikchains[i].sznameindex, ikchains[i].linktype, ikchains[i].numlinks, ikchains[i].linkindex, unk, 0, 0, 0 };
		v53IkChains.push_back(ikChain);
	}
	return v53IkChains;
}

void MDL::v49Mdl::HitboxSetConversion()
{
	int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;
	for (int i = 0; i < hitboxsets.size(); i++)
	{
		hitboxsets[i].sznameindex += stairs;
	}
}

void MDL::v49Mdl::AttachmentConversion()
{
	int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;
	for (int i = 0; i < attachments.size(); i++)
	{
		attachments[i].sznameindex += stairs;
	}
}

void MDL::v49Mdl::SeqEventConversion()
{
	int actModNum = 0;
	int eventNum = 0;
	for (int i = 0; i < seqdescs.size(); i++)
	{
		for (int j = 0; j < seqdescs[i].numevents; j++)
		{
			int stairs = 4 * (activitymodifiers.size() - actModNum) + bytesAddedToRuiMesh + bytesAddedToTextures + bytesAddedToIkChains + textureFiller + bytesAddedToLinearBone + bytesAddedToAABBTree;
			if (events[eventNum].szeventindex > 0) events[eventNum].szeventindex += stairs;
			eventNum++;
		}
		if (seqdescs[i].numactivitymodifiers > 0) actModNum += seqdescs[i].numactivitymodifiers;
	}
}

void MDL::v49Mdl::ConvertNodeNames()
{
	for (int i = 0; i < nodenames.size(); i++)
	{
		if (nodenames[i].sznameindex > 0) nodenames[i].sznameindex += allBytesAdded + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;
	}
}

void MDL::v49Mdl::ConvertBodyParts()
{
	int stairs = bytesAddedToTextures + bytesAddedToIkChains + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;
	for (int i = 0; i < bodyparts.size(); i++)
	{
		if (bodyparts[i].sznameindex > 0) bodyparts[i].sznameindex += stairs;
	}
}

void MDL::v49Mdl::ConvertModels()
{
	for (int i = 0; i < models.size(); i++)
	{
		models[i].eyeballindex = 0;
		models[i].unused[0] = 0;
		models[i].unused[1] = 0;
		if (mdlhdr.numikchains > 0)
		{
			if (models[i].meshindex > 0) models[i].meshindex += bytesAddedToIkChains;
			if (models[i].attachmentindex > 0) models[i].attachmentindex += bytesAddedToIkChains;
		}
	}
}

void MDL::v49Mdl::ConvertMeshes()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		if (mdlhdr.numikchains > 0)meshes[i].modelindex -= bytesAddedToIkChains;
		else break;
	}
}

void MDL::v49Mdl::ConvertPoseParamDescs()
{
	int stairs = bytesAddedToTextures + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;
	for (int i = 0; i < poseparamdescs.size(); i++)
	{
		if (poseparamdescs[i].sznameindex > 0) poseparamdescs[i].sznameindex += stairs;
	}
}

void MDL::v49Mdl::ConvertIncludeModel()
{
	int stairs = bytesAddedToTextures + textureFiller + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;
	for (int i = 0; i < includedmodels.size(); i++)
	{
		if (includedmodels[i].sznameindex > 0) includedmodels[i].sznameindex += stairs;
	}
}

std::vector<mstudiotexture_t_v53> MDL::v49Mdl::ConvertTextures()
{
	std::vector<mstudiotexture_t_v53> v53Textures;

	for (int i = 0; i < textures.size(); i++)
	{
		int stairs = -((20) * (textures.size() - i));
		textures[i].sznameindex += stairs + bytesAddedToLinearBone + bytesAddedToAABBTree;

		mstudiotexture_t_v53 texture = { textures[i].sznameindex, textures[i].flags, textures[i].used, textures[i].unused1, 0,0,0,0,0,0,0 };
		v53Textures.push_back(texture);
	}

	return v53Textures;
}

void MDL::v49Mdl::ConvertCDTextures()
{
	for (int i = 0; i < cdtextures.size(); i++)
	{
		cdtextures[i].sznameindex += allBytesAdded + bytesAddedToRuiMesh + bytesAddedToLinearBone + bytesAddedToAABBTree;
	}
}

void MDL::v49Mdl::ConvertSrcBoneTransforms()
{
	for (int i = 0; i < srcbonetransforms.size(); i++)
	{
		srcbonetransforms[i].sznameindex += bytesAddedToLinearBone + bytesAddedToAABBTree;
	}
}

void MDL::v49Mdl::ConvertLinearBone()
{
	linearbone.posscaleindex = 0;
	linearbone.rotscaleindex += bytesAddedToLinearBone;
	linearbone.qalignmentindex += bytesAddedToLinearBone;
}

mstudiolinearbonedata_t_v53 MDL::v49Mdl::ConvertLinearBoneData()
{
	mstudiolinearbonedata_t_v53 linearBoneData{};
	memcpy(linearBoneData.flags, linearbonedata.flags, sizeof(uint32_t) * mdlhdr.numbones);
	memcpy(linearBoneData.parents, linearbonedata.parents, sizeof(uint32_t) * mdlhdr.numbones);
	memcpy(linearBoneData.bonePos, linearbonedata.bonePos, sizeof(Vector3) * mdlhdr.numbones);
	memcpy(linearBoneData.boneQuat, linearbonedata.boneQuat, sizeof(Quaternion) * mdlhdr.numbones);
	memcpy(linearBoneData.boneRot, linearbonedata.boneRot, sizeof(RadianEuler) * mdlhdr.numbones);
	memcpy(linearBoneData.poseToBone, linearbonedata.poseToBone, sizeof(matrix3x4_t) * mdlhdr.numbones);
	memcpy(linearBoneData.rotScale, linearbonedata.rotScale, sizeof(Vector3) * mdlhdr.numbones);
	memcpy(linearBoneData.boneAlignment, linearbonedata.boneAlignment, sizeof(Quaternion) * mdlhdr.numbones);
	return linearBoneData;
}

void MDL::v49Mdl::UpdateMdl()
{
	if (hitboxsets.size() > 0) HitboxSetConversion();
	if (attachments.size() > 0) AttachmentConversion();
	if (events.size() > 0) SeqEventConversion();
	if (mdlhdr.numlocalnodes > 0) ConvertNodeNames();
	if (mdlhdr.numbodyparts > 0) ConvertBodyParts();
	if (models.size() > 0) ConvertModels();
	if (meshes.size() > 0) ConvertMeshes();
	if (mdlhdr.numlocalposeparameters > 0) ConvertPoseParamDescs();
	if (mdlhdr.numincludemodels > 0) ConvertIncludeModel();
	if (mdlhdr.numcdtextures > 0) ConvertCDTextures();
	if (mdlsubhdr.numsrcbonetransform > 0) ConvertSrcBoneTransforms();
	if (mdlhdr.numbones > 0) ConvertLinearBone();
}
#pragma endregion

#pragma region PHY

MDL::PHY MDL::PHY::_PHY(BinaryReader* Stream, bool debug)
{
	//Logger::Info("Test\n");
	MDL::PHY vPhy;
	//Logger::Info("Test\n");
	if (Stream->Stream.good())
	{
		//Logger::Info("Test2\n");
		phyheader_t					phyHdr; Stream->Read(&phyHdr);
		std::vector<physection_t>	phySecs;
		//Logger::Info("Test2\n");
		for (int i = 0; i < phyHdr.numsolids; i++)
		{
			//Logger::Info("Test2\n");
			physection_t phySec; Stream->Read(&phySec);
			phySecs.push_back(phySec);
		}
		//Logger::Info("Test3\n");
		MDL::PHY Phy{ phyHdr, phySecs };
		return Phy;
	}
	else
		return vPhy;
}

#pragma endregion
