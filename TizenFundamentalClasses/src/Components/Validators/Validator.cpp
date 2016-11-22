/*
 * Validator.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: Kevin Winata
 */

#include "TFC/Components/Validators/Validator.h"

LIBAPI
TFC::Components::Validators::Validator::Validator(ComponentBase* component) :
	component(component),
	ErrorMessage(errorMessage)
{

}

LIBAPI
TFC::Components::Validators::Validator::~Validator()
{
}

LIBAPI
bool TFC::Components::Validators::Validator::Validate()
{
	if (component->IsCreated())
		validationResult = ValidateInternal();
	return validationResult;
}

LIBAPI
TFC::Components::Validators::ValidatorList::ValidatorList() :
	LastValidationMessage(lastValidationMessage)
{
	lastValidationResult = true;
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
		if (!validator->Validate())
		{
			lastValidationResult = false;
			lastValidationMessage = validator->ErrorMessage;
			return false;
		}
	}
	return true;
}
