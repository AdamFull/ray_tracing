#include "vertex_buffer.h"

template<class _Ty>
void vector_append(std::vector<_Ty>& dst, const std::vector<_Ty>& src)
{
	dst.insert(dst.end(), src.begin(), src.end());
}

void CVertexBuffer::add_vertices(const std::vector<FVertex>& vertices)
{
	vector_append(m_vertices, vertices);
}

void CVertexBuffer::add_indices(const std::vector<uint32_t>& indices)
{
	vector_append(m_indices, indices);
}

void CVertexBuffer::add_mesh_data(std::unique_ptr<mesh_template>&& mesh_template)
{
	add_mesh_data(mesh_template->vertices, mesh_template->indices);
}

void CVertexBuffer::add_mesh_data(const std::vector<FVertex>& vertices, const std::vector<uint32_t>& indices)
{
	add_vertices(vertices);
	add_indices(indices);
}

const FVertex& CVertexBuffer::get_vertex(size_t index)
{
	return m_vertices.at(index);
}

uint32_t CVertexBuffer::get_index(size_t index)
{
	return m_indices.at(index);
}

size_t CVertexBuffer::get_last_vertex() const
{
	return m_vertices.size();
}

size_t CVertexBuffer::get_last_index() const
{
	return m_indices.size();
}

void CVertexBuffer::clear()
{
	m_vertices.clear();
	m_indices.clear();
}