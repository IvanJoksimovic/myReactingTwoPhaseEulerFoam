/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2014-2018 OpenFOAM Foundation
    Copyright (C) 2019-2020 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "liftModel.H"
#include "phasePair.H"
#include "fvcCurl.H"
#include "fvcFlux.H"
#include "surfaceInterpolate.H"
#include "BlendedInterfacialModel.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(liftModel, 0);
    defineBlendedInterfacialModelTypeNameAndDebug(liftModel, 0);
    defineRunTimeSelectionTable(liftModel, dictionary);
}

const Foam::dimensionSet Foam::liftModel::dimF(1, -2, -2, 0, 0);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::liftModel::liftModel
(
    const dictionary& dict,
    const phasePair& pair
)
:
    pair_(pair)
{}


// * * * * * * * * * * * * * * * * Selector  * * * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::liftModel>
Foam::liftModel::New
(
    const dictionary& dict,
    const phasePair& pair
)
{
    const word modelType(dict.get<word>("type"));

    Info<< "Selecting liftModel for "
        << pair << ": " << modelType << endl;

    auto cstrIter = dictionaryConstructorTablePtr_->cfind(modelType);

    if (!cstrIter.found())
    {
        FatalIOErrorInLookup
        (
            dict,
            "liftModel",
            modelType,
            *dictionaryConstructorTablePtr_
        ) << abort(FatalIOError);
    }

    return cstrIter()(dict, pair);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::volVectorField> Foam::liftModel::Fi() const
{
    return
        Cl()
       *pair_.continuous().rho()
       *(
            pair_.Ur() ^ fvc::curl(pair_.continuous().U())
        );
}


Foam::tmp<Foam::volVectorField> Foam::liftModel::F() const
{
    return pair_.dispersed()*Fi();
}


Foam::tmp<Foam::surfaceScalarField> Foam::liftModel::Ff() const
{
    return fvc::interpolate(pair_.dispersed())*fvc::flux(Fi());
}


// ************************************************************************* //
