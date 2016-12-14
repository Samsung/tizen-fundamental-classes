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
	ErrorMessage(errorMessage),
	component(component),
	validationResult(Validator::ERROR_NONE)
{
	errorDictionary[Validator::ERROR_NONE] = "No error found in component %0.";
	errorDictionary[Validator::ERROR_INVALID_COMPONENT] = "Component %0 is not valid / created yet.";

	formatFunctions.push_back([component] () { std::string n = component->Name; return n; });
}

LIBAPI
TFC::Components::Validators::Validator::~Validator()
{
}

LIBAPI
int TFC::Components::Validators::Validator::Validate()
{
	if (component->IsCreated())
		validationResult = ValidateInternal();
	else
		validationResult = Validator::ERROR_INVALID_COMPONENT;

	GenerateErrorMessage();
	return validationResult;
}

LIBAPI void TFC::Components::Validators::Validator::SetErrorMessageFormat(int error, std::string const& message)
{
	errorDictionary[error] = message;
}

void TFC::Components::Validators::Validator::GenerateErrorMessage()
{
	errorMessage = errorDictionary[validationResult];
	for (std::size_t i = 0; i < formatFunctions.size(); i++)
	{
		std::regex rgx("\%" + std::to_string(i));
		auto const& func = formatFunctions[i];
		errorMessage = std::regex_replace(errorMessage, rgx, func());
	}
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
