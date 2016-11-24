/*
 * Validator.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: Kevin Winata
 */

#include "TFC/Components/Validators/Validator.h"

using namespace TFC::Components::Validators;

int const Validator::ERROR_NONE = -1;
int const Validator::ERROR_INVALID_COMPONENT = 0;

LIBAPI
TFC::Components::Validators::Validator::Validator(ComponentBase* component) :
	component(component),
	validationResult(Validator::ERROR_NONE),
	ErrorMessage(errorMessage)
{
	errorDictionary[Validator::ERROR_NONE] = "No error found.";
	errorDictionary[Validator::ERROR_INVALID_COMPONENT] = "Component is not valid / created yet.";
}

LIBAPI
TFC::Components::Validators::Validator::~Validator()
{
}

LIBAPI
int TFC::Components::Validators::Validator::Validate()
{
	if (component->IsCreated())
		return validationResult = ValidateInternal();
	else
		return validationResult = Validator::ERROR_INVALID_COMPONENT;
}

LIBAPI void TFC::Components::Validators::Validator::SetErrorMessageFormat(int error, std::string const& message)
{
	errorDictionary[error] = message;
}

LIBAPI
TFC::Components::Validators::ValidatorList::ValidatorList() :
	LastValidationMessage(lastValidationMessage)
{
	lastValidationResult = Validator::ERROR_NONE;
}

LIBAPI
TFC::Components::Validators::ValidatorList::~ValidatorList()
{
	for (auto validator : validators)
		delete validator;
}

LIBAPI
void TFC::Components::Validators::ValidatorList::AddValidator(Validator* validator)
{
	validators.push_back(validator);
}

LIBAPI
bool TFC::Components::Validators::ValidatorList::ValidateAll()
{
	for (auto validator : validators)
	{
		if (auto result = validator->Validate() != Validator::ERROR_NONE)
		{
			lastValidationResult = result;
			lastValidationMessage = validator->ErrorMessage;
			return false;
		}
	}
	return true;
}
