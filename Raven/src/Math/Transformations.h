#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/ext.hpp"


namespace raven
{
	enum TransformationType
	{
		None,
		Translation,
		Scaling,
		Rotation,
		Composite
	};

	class Transformations
	{
	public:
		Transformations() {}
		
		Transformations(glm::mat4 transformation) // TODO: refactor it
		{
			m_id = 0;
			m_type = TransformationType::Composite;
			m_angle = 0.0f;
			m_vector = glm::vec3(0.0f);
			m_transformations = transformation;

			m_inverse_transformations = glm::inverse(m_transformations);
			
			m_normal_transformations = glm::inverseTranspose(m_transformations);

		}
		// Constructor for Translation and Scaling
		Transformations(int _id, TransformationType _type, glm::vec3 _transformation_vec) :
			m_id(_id), m_type(_type), m_vector(_transformation_vec), m_transformations(glm::mat4(1.0f)), m_inverse_transformations(glm::mat4(1.0f)), m_normal_transformations(glm::mat4(1.0f)), m_angle(0.0f)
		{
			if (_type == TransformationType::Translation)
			{
				m_transformations = (glm::translate(m_transformations, _transformation_vec));
				m_inverse_transformations = glm::inverse(m_transformations);

				m_normal_transformations = glm::inverseTranspose(m_transformations);
			}
			
			if (_type == TransformationType::Scaling)
			{
				m_transformations = (glm::scale(m_transformations, _transformation_vec));

				m_inverse_transformations = glm::inverse(m_transformations);
				m_normal_transformations = glm::inverseTranspose(m_transformations);

			}
		
		}

		// Constructor for Rotation
		Transformations(int _id, TransformationType _type, float _angle, glm::vec3 _transformation_vec) :
			m_id(_id), m_type(_type), m_angle(_angle), m_vector(_transformation_vec), m_transformations(glm::mat4(1.0f)), m_inverse_transformations(glm::mat4(1.0f))
		{
			if (_type == TransformationType::Rotation) // TODO: is it redundant?
			{
				m_transformations = (glm::rotate(m_transformations, glm::radians(_angle), _transformation_vec));
				
				m_inverse_transformations = glm::inverse(m_transformations);
				m_normal_transformations = glm::inverseTranspose(m_transformations);

			}
		}
		

	public:
		int m_id;
		TransformationType m_type;
		float m_angle;
		glm::vec3 m_vector;
		glm::mat4 m_transformations;
		glm::mat4 m_inverse_transformations; 
		glm::mat4 m_normal_transformations; // can be mat3
		// TODO: add normal_transformation
	};
}
