#include "GeometryNode.h"
#include "GeometricMesh.h"
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include "TextureManager.h"


GeometryNode::GeometryNode()
{
	m_vao = 0;
	m_vbo_positions = 0;
	m_vbo_normals = 0;
	m_vbo_texcoords = 0;
	m_vbo_tangents = 0;
	m_vbo_bitangents = 0;
}

GeometryNode::~GeometryNode()
{
	// delete buffers
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo_positions);
	glDeleteBuffers(1, &m_vbo_normals);
	glDeleteBuffers(1, &m_vbo_texcoords);
	glDeleteBuffers(1, &m_vbo_tangents);
	glDeleteBuffers(1, &m_vbo_bitangents);
}

void GeometryNode::Init(GeometricMesh* mesh)
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(glm::vec3), &mesh->vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,				// attribute index
		3,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,       // the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	glGenBuffers(1, &m_vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(glm::vec3), &mesh->normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,				// attribute index
		3,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,		// the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	if (!mesh->textureCoord.empty())
	{
		glGenBuffers(1, &m_vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, mesh->textureCoord.size() * sizeof(glm::vec2), &mesh->textureCoord[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,				// attribute index
			2,              // number of elements per vertex, here (x,y,z)
			GL_FLOAT,		// the type of each element
			GL_FALSE,       // take our values as-is
			0,		        // no extra data between each position
			0				// pointer to the C array or an offset to our buffer
		);

		//TODO: PROBABLY HAVE TO ADD HERE CODE FOR NORMAL MAPPING
		//--------------- Normal mapping code - Begin------------------------

		glGenBuffers(1, &m_vbo_tangents);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_tangents);
		glBufferData(GL_ARRAY_BUFFER, mesh->tangents.size() * sizeof(glm::vec3), &mesh->tangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3,				// attribute index
			3,              // number of elements per vertex, here (x,y,z)
			GL_FLOAT,		// the type of each element
			GL_FALSE,       // take our values as-is
			0,		         // no extra data between each position
			0				// pointer to the C array or an offset to our buffer
		);

		glGenBuffers(1, &m_vbo_bitangents);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bitangents);
		glBufferData(GL_ARRAY_BUFFER, mesh->bitangents.size() * sizeof(glm::vec3), &mesh->bitangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(
			4,				// attribute index
			3,              // number of elements per vertex, here (x,y,z)
			GL_FLOAT,		// the type of each element
			GL_FALSE,       // take our values as-is
			0,		         // no extra data between each position
			0				// pointer to the C array or an offset to our buffer
		);

		glGenBuffers(1, &m_vbo_bitangents);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bitangents);
		glBufferData(GL_ARRAY_BUFFER, mesh->bitangents.size() * sizeof(glm::vec3), &mesh->bitangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(
			5,				// attribute index
			1,              // number of elements per vertex, here (x,y,z)
			GL_FLOAT,		// the type of each element
			GL_FALSE,       // take our values as-is
			0,		         // no extra data between each position
			0				// pointer to the C array or an offset to our buffer
		);

		//--------------- Normal mapping code - End--------------------------
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// *********************************************************************

	for (int i = 0; i < mesh->objects.size(); i++)
	{
		Objects part;
		part.start_offset = mesh->objects[i].start;
		part.count = mesh->objects[i].end - mesh->objects[i].start;
		auto material = mesh->materials[mesh->objects[i].material_id];
		
		part.diffuseColor = glm::vec4(material.diffuse[0], material.diffuse[1], material.diffuse[2], 1.0);
		part.specularColor = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
		part.shininess = material.shininess;
		part.textureID = (material.texture.empty())? 0 : TextureManager::GetInstance().RequestTexture(material.texture.c_str());

		parts.push_back(part);
	}

	renderMode = GL_TRIANGLES;
}

void GeometryNode::InitLightning()
{
	std::vector<float> vertices;
	vertices.reserve(303);

	glm::vec3 direction = glm::vec3(20.0, 0, 20.0);

	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(0, 0.3);

	vertices.push_back(0.0);
	vertices.push_back(0.0);
	vertices.push_back(0.0);

	for (int i = 1; i < 100; ++i)
	{
		vertices.push_back(double(i) / 100.0 * direction.x);
		vertices.push_back(double(i) / 100.0 * direction.y);
		vertices.push_back(double(i) / 100.0 * direction.z);
	}
	vertices.push_back(20.0);
	vertices.push_back(0.0);
	vertices.push_back(20.0);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,				// attribute index
		3,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,       // the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	Objects part;
	part.start_offset = 0;
	part.count = 100;

	part.diffuseColor = glm::vec4(0.0, 2.0, 1.0, 1.0);
	part.specularColor = glm::vec3(0.0, 2.0, 1.0);
	part.shininess = 1;
	part.textureID = 0;

	parts.push_back(part);

	renderMode = GL_LINE_STRIP;
}
