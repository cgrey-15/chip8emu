#ifndef MULTI_ARRAY_H
#define MULTI_ARRAY_H

#include <array>
#include <span>

template <typename V, uint16_t ROW, uint16_t COL>
class MultiArray {
		public:

				std::span<V, COL> operator[](const size_t r) { return std::span<V,COL>{&data_[r*COL], COL}; }
				
				inline size_t rowLength() const {return ROW;}
				inline size_t colLength() const { return COL; }

				V * data() { return data_.data(); }
				const V* cdata() const { return data_.data(); }
		private:
				std::array<V, ROW*COL> data_;
};

#endif // MULTI_ARRAY_H
