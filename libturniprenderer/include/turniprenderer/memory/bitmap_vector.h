#pragma once

namespace TurnipRenderer::Memory {
	// Allocates contiguous "buckets" of 64 * sizeof(T)
	// Uses a uint64_t for each bucket to determine which values are being used active
	// Order not guaranteed, if some object in the middle is removed then the next created one will go there
	// TODO: Consider a order-guaranteed version using a vector of pointers. Allocation remains unordered but begin() and end() have consistent ordering due to vector's behaviour.
	/*template<typename T>
	class BitmapSet {
	public:
		BitmapSet(int initialBucketCount){
			buckets.resize(initialBucketCount);
		}

		struct iterator : public std::iterator<std::forward_iterator_tag, T> {
			size_t bucketIndex;
			uint8_t indexWithinBucket;

			
		};
		// TODO: begin() and end(), iterators
		// operator[] would be pretty inefficient
	private:
		struct Bucket {
			uint64_t availability = 0; // Bit N = 0 if Nth part of bucket is not in use
			T objects[BucketSize];
		};
		std::vector<std::unique_ptr<Bucket>> buckets;
	};*/
}
