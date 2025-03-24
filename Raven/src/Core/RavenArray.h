#pragma once

namespace raven
{
	// Raven Array class

	template <class T> class RavenArray
	{
	public:
		RavenArray(); // Default constructor
		RavenArray(int); // Constructor with size
		~RavenArray(); // Destructor

		bool append(T item); // Append an item to the array
		//bool move(T&& item); // append with move 
		bool truncate(); // Truncate the array to the number of elements
		void clear() { m_num_element = 0; } // Clear the array
		int size() const { return m_num_element; } // Return the number of elements in the array
		bool empty() const { return m_num_element == 0; } // Return true if the array is empty
		const T& operator[] (int i) const { return m_data[i]; } // Return the element at index i
		T& operator[] (int i) { return m_data[i]; } // Return the element at index i

	public:
		T* m_data; // Array
		int m_num_element; // number of available element in the array
		int m_size; // Array size
	};

	template <class T> RavenArray<T>::RavenArray()
	{
		m_num_element = 0;
		m_size = 4;
		m_data = new T[m_size];
	}

	template <class T> RavenArray<T>::RavenArray(int size)
	{
		m_num_element = 0;
		m_size = size;
		m_data = new T[m_size];
	}

	template <class T> RavenArray<T>::~RavenArray()
	{
		m_num_element = 0;
		m_size = 0;
		delete [] m_data;
	}

	template <class T> bool RavenArray<T>::truncate()
	{
		if (m_num_element != m_size)
		{
			T* temp = m_data;
			m_size = m_num_element;

			if (!(m_data = new T[m_size]))
				return false;

			for (int i = 0; i < m_num_element; i++)
				m_data[i] = temp[i];

			delete[] temp;
		}
		return true;
	}

	template <class T> bool RavenArray<T>::append(T item)
	{
		if (m_size == m_num_element)
		{
			m_size *= 2;
			T* temp = m_data;

			if (!(m_data = new T[m_size]))
				return false;

			for (int i = 0; i < m_num_element; i++)
				m_data[i] = temp[i];

			delete[] temp;
		}

		if (m_num_element < m_size) // Ensure there is space before assignment
		{
			m_data[m_num_element++] = item;
			return true;
		}
		return false; // Return false if there is no space (should not happen due to resizing)
	}

	//template <class T>	bool RavenArray<T>::move(T&& item)
	//{
	//	if (m_size == m_num_element)
	//	{
	//		m_size *= 2;
	//		T* temp = m_data;

	//		if (!(m_data = new T[m_size]))
	//			return false;

	//		for (int i = 0; i < m_num_element; i++)
	//			m_data[i] = std::move(temp[i]); // Move elements instead of copying

	//		delete[] temp;
	//	}

	//	if (m_num_element < m_size) // Ensure there is space before assignment
	//	{
	//		m_data[m_num_element++] = std::move(item); // Use move for assignment
	//		return true;
	//	}

	//	return false; // Return false if there is no space (should not happen due to resizing)
	//}
}
