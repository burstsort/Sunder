#include "Model.hpp"

#include <iostream>

Model::Model(Mesh* mesh, Texture* texOut, Texture* texIn)
{
	_mesh = mesh;
	_texOut = texOut;
	_texIn = texIn;

	frozen = false;

	mat4x4_identity(_transform);

	_fragments.push_back(new Fragment(_mesh->GetIndices()));
}

Model::~Model()
{
	for (unsigned int i = 0; i < _fragments.size(); ++i)
	{
		delete _fragments[i];
	}
}

void Model::ApplyTransform(mat4x4 transform)
{
	mat4x4 mat;
	for (unsigned int i = 0; i < _fragments.size(); ++i)
	{
		_fragments[i]->GetTransform(mat);
		mat4x4_mul(mat, transform, mat);
		_fragments[i]->SetTransform(mat);
	}

	mat4x4_mul(_transform, _transform, transform);
}

void Model::UpdatePhysics(double dt)
{
	for (unsigned int i = 0; i < _fragments.size(); ++i)
	{
		_fragments[i]->UpdatePhysics(dt);
	}
}

void Model::Render(GLint program)
{
	glBindVertexArray(_mesh->GetVAO());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texOut->GetId());
	glUniform1i(glGetUniformLocation(program, "texOut"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _texIn->GetId());
	glUniform1i(glGetUniformLocation(program, "texIn"), 1);

	for (unsigned int i = 0; i < _fragments.size(); ++i)
	{
		_fragments[i]->Render(program);
	}
}

void Model::RenderWireframe(GLint program)
{
	glBindVertexArray(_mesh->GetVAO());

	for (unsigned int i = 0; i < _fragments.size(); ++i)
	{
		_fragments[i]->RenderWireframe(program);
	}
}

void Model::Freeze()
{
	frozen = true;
}

void Model::Unfreeze()
{
	frozen = false;
}

bool Model::IsFrozen()
{
	return frozen;
}

void Model::Slice(vec3 point, vec3 normal, float separation)
{
	if (frozen)
		return;__gl3w_h_

	vec4 p, pt, n, nt;
	mat4x4 T, Ti;
	vec3 speed;

	// Iterate through fragments
	unsigned int count = _fragments.size();
	for (unsigned int i = 0; i < count; ++i)
	{		
		p[0] = point[0]; p[1] = point[1]; p[2] = point[2]; p[3] = 1.0f;
		n[0] = normal[0]; n[1] = normal[1]; n[2] = normal[2]; n[3] = 0.0f;

		_fragments[i]->GetSpeed(speed);
		_fragments[i]->GetTransform(T);
		mat4x4_invert(Ti, T);
		
		mat4x4_mul_vec4(pt, Ti, p);
		mat4x4_mul_vec4(nt, Ti, n);
		nt[3] = 0.0f;

		vec4_norm(nt, nt);

		// Create new fragments
		_fragments.push_back(new Fragment(_mesh->GetIndices()));
		_fragments.push_back(new Fragment(_mesh->GetIndices()));

		// Slice the current fragment
		_mesh->Slice(_fragments[i]->GetIndices(), pt, nt, _fragments[2*i+count]->GetIndices(), _fragments[2*i+count+1]->GetIndices());

		// Update new fragments
		_fragments[2*i+count]->UpdateIndices();
		_fragments[2*i+count+1]->UpdateIndices();
		_fragments[2*i+count]->SetTransform(T);
		_fragments[2*i+count+1]->SetTransform(T);
		_fragments[2*i+count]->AddSpeed(speed);
		_fragments[2*i+count+1]->AddSpeed(speed);

		// Separate new fragments
		vec3_scale(speed, nt, separation);
		_fragments[2*i+count]->AddSpeed(speed);
		vec3_scale(speed, speed, -1.0f);
		_fragments[2*i+count+1]->AddSpeed(speed);
	}

	//Delete old fragments
	_fragments.erase(_fragments.begin(), _fragments.begin() + count);

	// Delete empty fragments
	for (unsigned int i = 0; i < _fragments.size(); ++i)
	{
		if (!_fragments[i]->GetPolycount())
		{
			delete _fragments[i];
			_fragments.erase(_fragments.begin()+i);
			i--;
		}
	}
}

unsigned int Model::Count()
{
	return _fragments.size();
}

void Model::Reset()
{
	for (unsigned int i = 0; i < _fragments.size(); ++i)
	{
		delete _fragments[i];
	}

	_fragments.clear();
	_fragments.push_back(new Fragment(_mesh->GetIndices()));
	_fragments[0]->SetTransform(_transform);
}
