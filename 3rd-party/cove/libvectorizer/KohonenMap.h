/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_KOHONENMAP_H
#define COVE_KOHONENMAP_H

#include <memory>
#include <vector>

namespace cove {
class OrganizableElement
{
public:
	virtual ~OrganizableElement();
	virtual OrganizableElement* clone() const = 0;
	virtual double distance(const OrganizableElement& y) const = 0;
	virtual double squares(const OrganizableElement& y) const = 0;
	virtual void add(const OrganizableElement& y) = 0;
	virtual void subtract(const OrganizableElement& y) = 0;
	virtual void multiply(double y) = 0;
};

class KohonenAlphaGetter
{
public:
	virtual ~KohonenAlphaGetter();
	virtual double getAlpha() = 0;
	virtual unsigned int getE() = 0;
};

class KohonenPatternGetter
{
public:
	virtual ~KohonenPatternGetter();
	virtual const OrganizableElement* getPattern() = 0;
};

class BatchPatternGetter : public KohonenPatternGetter
{
public:
	~BatchPatternGetter() override;
	virtual int getLastElementClass() const = 0;
	virtual void setLastElementClass(int classNumber) = 0;
	virtual void reset() = 0;
	virtual int numberOfChanges() = 0;
};

class KohonenMap
{
	std::vector<std::unique_ptr<OrganizableElement>> classes;

public:
	void setClasses(const std::vector<OrganizableElement*>& newClasses);
	std::vector<std::unique_ptr<OrganizableElement>> getClasses();
	int findClosest(const OrganizableElement& v, double& bestDistance) const;
	void learn(const OrganizableElement& v, double alfa);
	void performLearning(KohonenAlphaGetter& alphaGetter,
	                     KohonenPatternGetter& patternGetter);
	double performBatchLearning(BatchPatternGetter& patternGetter);
};
} // cove

#endif
