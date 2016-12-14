/*
 * Validator.h
 *
 *  Created on: Nov 22, 2016
 *      Author: Kevin Winata
 */

#ifndef TFC_COMPONENTS_VALIDATORS_VALIDATOR_H_
#define TFC_COMPONENTS_VALIDATORS_VALIDATOR_H_

#include "TFC/Components/ComponentBase.h"
#include <vector>
#include <unordered_map>
#include <regex>
#include <functional>

namespace TFC {
namespace Components {
namespace Validators {

class Validator {
public:
	Validator(ComponentBase* component);
	virtual ~Validator();

	static int const ERROR_NONE;
	static int const ERROR_INVALID_COMPONENT;
	std::string const& ErrorMessage;

	int Validate();
	void SetErrorMessageFormat(int error, std::string const& message);
protected:
	std::unordered_map<int, std::string> errorDictionary;
	std::vector<std::function<std::string()>> formatFunctions;

	ComponentBase* const component;
	int validationResult;
	virtual int ValidateInternal() = 0;
private:
	std::string errorMessage;
	void GenerateErrorMessage();
};

class ValidatorList {
public:
	ValidatorList();
	~ValidatorList();
	void AddValidator(Validator* validator);
	bool ValidateAll();
	const std::string& LastValidationMessage;
private:
	std::string lastValidationMessage;
	std::vector<Validator*> validators;
	int lastValidationResult;
};

}
}
}



#endif /* TFC_COMPONENTS_VALIDATORS_VALIDATOR_H_ */
