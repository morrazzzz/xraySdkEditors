// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WML) source code is supplied under the terms of
// the license agreement http://www.magic-software.com/License/WildMagic.pdf
// and may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real>::GMatrix(int iRows, int iCols)
{
    m_afData = 0;
    m_aafEntry = 0;
    SetSize(iRows, iCols);
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real>::GMatrix(int iRows, int iCols, const Real *afEntry)
{
    m_afData = 0;
    m_aafEntry = 0;
    SetMatrix(iRows, iCols, afEntry);
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real>::GMatrix(int iRows, int iCols, const Real **aafMatrix)
{
    m_afData = 0;
    m_aafEntry = 0;
    SetMatrix(iRows, iCols, aafMatrix);
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real>::GMatrix(const GMatrix &rkM)
{
    m_iRows = 0;
    m_iCols = 0;
    m_iQuantity = 0;
    m_afData = 0;
    m_aafEntry = 0;
    *this = rkM;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real>::~GMatrix()
{
    Deallocate();
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::Allocate(bool bSetToZero)
{
    // assert:  m_iRows, m_iCols, and m_iQuantity already initialized

    m_afData = new Real[m_iQuantity];
    if (bSetToZero)
        memset(m_afData, 0, m_iQuantity * sizeof(Real));

    m_aafEntry = new Real *[m_iRows];
    for (int iRow = 0; iRow < m_iRows; iRow++)
        m_aafEntry[iRow] = &m_afData[iRow * m_iCols];
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::Deallocate()
{
    delete[] m_afData;
    delete[] m_aafEntry;
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::SetSize(int iRows, int iCols)
{
    Deallocate();
    if (iRows > 0 && iCols > 0)
    {
        m_iRows = iRows;
        m_iCols = iCols;
        m_iQuantity = m_iRows * m_iCols;
        Allocate(true);
    }
    else
    {
        m_iRows = 0;
        m_iCols = 0;
        m_iQuantity = 0;
        m_afData = 0;
        m_aafEntry = 0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::GetSize(int &riRows, int &riCols) const
{
    riRows = m_iRows;
    riCols = m_iCols;
}
//----------------------------------------------------------------------------
template <class Real>
int GMatrix<Real>::GetRows() const
{
    return m_iRows;
}
//----------------------------------------------------------------------------
template <class Real>
int GMatrix<Real>::GetColumns() const
{
    return m_iCols;
}
//----------------------------------------------------------------------------
template <class Real>
int GMatrix<Real>::GetQuantity() const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real>::operator const Real *() const
{
    return m_afData;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real>::operator Real *()
{
    return m_afData;
}
//----------------------------------------------------------------------------
template <class Real>
const Real *GMatrix<Real>::operator[](int iRow) const
{
    assert(0 <= iRow && iRow < m_iRows);
    return m_aafEntry[iRow];
}
//----------------------------------------------------------------------------
template <class Real>
Real *GMatrix<Real>::operator[](int iRow)
{
    assert(0 <= iRow && iRow < m_iRows);
    return m_aafEntry[iRow];
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::SwapRows(int iRow0, int iRow1)
{
    assert(0 <= iRow0 && iRow0 < m_iRows && 0 <= iRow1 && iRow1 < m_iRows);
    Real *afSave = m_aafEntry[iRow0];
    m_aafEntry[iRow0] = m_aafEntry[iRow1];
    m_aafEntry[iRow1] = afSave;
}
//----------------------------------------------------------------------------
template <class Real>
Real GMatrix<Real>::operator()(int iRow, int iCol) const
{
    return m_aafEntry[iRow][iCol];
}
//----------------------------------------------------------------------------
template <class Real>
Real &GMatrix<Real>::operator()(int iRow, int iCol)
{
    assert(0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol <= m_iCols);
    return m_aafEntry[iRow][iCol];
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::SetRow(int iRow, const GVector<Real> &rkV)
{
    assert((0 <= iRow && iRow < m_iRows) && (rkV.GetSize() == m_iCols));
    for (int iCol = 0; iCol < m_iCols; iCol++)
        m_aafEntry[iRow][iCol] = rkV[iCol];
}
//----------------------------------------------------------------------------
template <class Real>
GVector<Real> GMatrix<Real>::GetRow(int iRow) const
{
    assert(0 <= iRow && iRow < m_iRows);
    GVector<Real> kV(m_iCols);
    for (int iCol = 0; iCol < m_iCols; iCol++)
        kV[iCol] = m_aafEntry[iRow][iCol];
    return kV;
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::SetColumn(int iCol, const GVector<Real> &rkV)
{
    assert((0 <= iCol && iCol < m_iCols) && (rkV.GetSize() == m_iRows));
    for (int iRow = 0; iRow < m_iRows; iRow++)
        m_aafEntry[iRow][iCol] = rkV[iRow];
}
//----------------------------------------------------------------------------
template <class Real>
GVector<Real> GMatrix<Real>::GetColumn(int iCol) const
{
    assert(0 <= iCol && iCol < m_iCols);
    GVector<Real> kV(m_iRows);
    for (int iRow = 0; iRow < m_iRows; iRow++)
        kV[iRow] = m_aafEntry[iRow][iCol];
    return kV;
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::SetMatrix(int iRows, int iCols, const Real *afData)
{
    Deallocate();
    if (iRows > 0 && iCols > 0)
    {
        m_iRows = iRows;
        m_iCols = iCols;
        m_iQuantity = m_iRows * m_iCols;
        Allocate(false);
        memcpy(m_afData, afData, m_iQuantity * sizeof(Real));
    }
    else
    {
        m_iRows = 0;
        m_iCols = 0;
        m_iQuantity = 0;
        m_afData = 0;
        m_aafEntry = 0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::SetMatrix(int iRows, int iCols, const Real **aafEntry)
{
    Deallocate();
    if (iRows > 0 && iCols > 0)
    {
        m_iRows = iRows;
        m_iCols = iCols;
        m_iQuantity = m_iRows * m_iCols;
        Allocate(false);
        for (int iRow = 0; iRow < m_iRows; iRow++)
        {
            for (int iCol = 0; iCol < m_iCols; iCol++)
                m_aafEntry[iRow][iCol] = aafEntry[iRow][iCol];
        }
    }
    else
    {
        m_iRows = 0;
        m_iCols = 0;
        m_iQuantity = 0;
        m_afData = 0;
        m_aafEntry = 0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void GMatrix<Real>::GetColumnMajor(Real *afCMajor) const
{
    for (int iRow = 0, i = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
            afCMajor[i++] = m_aafEntry[iCol][iRow];
    }
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> &GMatrix<Real>::operator=(const GMatrix &rkM)
{
    if (rkM.m_iQuantity > 0)
    {
        if (m_iRows != rkM.m_iRows || m_iCols != rkM.m_iCols)
        {
            Deallocate();
            m_iRows = rkM.m_iRows;
            m_iCols = rkM.m_iCols;
            m_iQuantity = rkM.m_iQuantity;
            Allocate(false);
        }
        for (int iRow = 0; iRow < m_iRows; iRow++)
        {
            for (int iCol = 0; iCol < m_iCols; iCol++)
                m_aafEntry[iRow][iCol] = rkM.m_aafEntry[iRow][iCol];
        }
    }
    else
    {
        Deallocate();
        m_iRows = 0;
        m_iCols = 0;
        m_iQuantity = 0;
        m_afData = 0;
        m_aafEntry = 0;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
bool GMatrix<Real>::operator==(const GMatrix &rkM) const
{
    return memcmp(m_afData, rkM.m_afData, m_iQuantity * sizeof(Real)) == 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool GMatrix<Real>::operator!=(const GMatrix &rkM) const
{
    return memcmp(m_afData, rkM.m_afData, m_iQuantity * sizeof(Real)) != 0;
}
//----------------------------------------------------------------------------
template <class Real>
int GMatrix<Real>::CompareArrays(const GMatrix &rkM) const
{
    return memcmp(m_afData, rkM.m_afData, m_iQuantity * sizeof(Real));
}
//----------------------------------------------------------------------------
template <class Real>
bool GMatrix<Real>::operator<(const GMatrix &rkM) const
{
    return CompareArrays(rkM) < 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool GMatrix<Real>::operator<=(const GMatrix &rkM) const
{
    return CompareArrays(rkM) <= 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool GMatrix<Real>::operator>(const GMatrix &rkM) const
{
    return CompareArrays(rkM) > 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool GMatrix<Real>::operator>=(const GMatrix &rkM) const
{
    return CompareArrays(rkM) >= 0;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> GMatrix<Real>::operator+(const GMatrix &rkM) const
{
    GMatrix<Real> kSum(rkM.m_iRows, rkM.m_iCols);
    for (int i = 0; i < m_iQuantity; i++)
        kSum.m_afData[i] = m_afData[i] + rkM.m_afData[i];
    return kSum;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> GMatrix<Real>::operator-(const GMatrix &rkM) const
{
    GMatrix<Real> kDiff(rkM.m_iRows, rkM.m_iCols);
    for (int i = 0; i < m_iQuantity; i++)
        kDiff.m_afData[i] = m_afData[i] - rkM.m_afData[i];
    return kDiff;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> GMatrix<Real>::operator*(const GMatrix &rkM) const
{
    // 'this' is RxN, 'M' is NxC, 'product = this*M' is RxC
    assert(m_iCols == rkM.m_iRows);
    GMatrix<Real> kProd(m_iRows, rkM.m_iCols);
    for (int iRow = 0; iRow < kProd.m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < kProd.m_iCols; iCol++)
        {
            for (int iMid = 0; iMid < m_iCols; iMid++)
            {
                kProd.m_aafEntry[iRow][iCol] += m_aafEntry[iRow][iMid] *
                                                rkM.m_aafEntry[iMid][iCol];
            }
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> GMatrix<Real>::operator*(Real fScalar) const
{
    GMatrix<Real> kProd(m_iRows, m_iCols);
    for (int i = 0; i < m_iQuantity; i++)
        kProd.m_afData[i] = fScalar * m_afData[i];
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> GMatrix<Real>::operator/(Real fScalar) const
{
    GMatrix<Real> kQuot(m_iRows, m_iCols);
    int i;

    if (fScalar != (Real)0.0)
    {
        Real fInvScalar = ((Real)1.0) / fScalar;
        for (i = 0; i < m_iQuantity; i++)
            kQuot.m_afData[i] = fInvScalar * m_afData[i];
    }
    else
    {
        for (i = 0; i < m_iQuantity; i++)
            kQuot.m_afData[i] = Math<Real>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> GMatrix<Real>::operator-() const
{
    GMatrix<Real> kNeg(m_iRows, m_iCols);
    for (int i = 0; i < m_iQuantity; i++)
        kNeg.m_afData[i] = -m_afData[i];
    return kNeg;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> Wml::operator*(Real fScalar, const GMatrix<Real> &rkM)
{
    GMatrix<Real> kProd(rkM.GetRows(), rkM.GetColumns());
    const Real *afMEntry = rkM;
    Real *afPEntry = kProd;
    for (int i = 0; i < rkM.GetQuantity(); i++)
        afPEntry[i] = fScalar * afMEntry[i];
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> &GMatrix<Real>::operator+=(const GMatrix &rkM)
{
    for (int i = 0; i < m_iQuantity; i++)
        m_afData[i] += rkM.m_afData[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> &GMatrix<Real>::operator-=(const GMatrix &rkM)
{
    for (int i = 0; i < m_iQuantity; i++)
        m_afData[i] -= rkM.m_afData[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> &GMatrix<Real>::operator*=(Real fScalar)
{
    for (int i = 0; i < m_iQuantity; i++)
        m_afData[i] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> &GMatrix<Real>::operator/=(Real fScalar)
{
    int i;

    if (fScalar != (Real)0.0)
    {
        Real fInvScalar = ((Real)1.0) / fScalar;
        for (i = 0; i < m_iQuantity; i++)
            m_afData[i] *= fInvScalar;
    }
    else
    {
        for (i = 0; i < m_iQuantity; i++)
            m_afData[i] = Math<Real>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> GMatrix<Real>::Transpose() const
{
    GMatrix<Real> kTranspose(m_iCols, m_iRows);
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
            kTranspose.m_aafEntry[iCol][iRow] = m_aafEntry[iRow][iCol];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> GMatrix<Real>::TransposeTimes(const GMatrix &rkM) const
{
    // P = A^T*B, P[r][c] = sum_m A[m][r]*B[m][c]
    assert(m_iRows == rkM.m_iRows);
    GMatrix<Real> kProd(m_iCols, rkM.m_iCols);
    for (int iRow = 0; iRow < kProd.m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < kProd.m_iCols; iCol++)
        {
            for (int iMid = 0; iMid < m_iRows; iMid++)
            {
                kProd.m_aafEntry[iRow][iCol] += m_aafEntry[iMid][iRow] *
                                                rkM.m_aafEntry[iMid][iCol];
            }
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
GMatrix<Real> GMatrix<Real>::TimesTranspose(const GMatrix &rkM) const
{
    // P = A*B^T, P[r][c] = sum_m A[r][m]*B[c][m]
    assert(m_iCols == rkM.m_iCols);
    GMatrix<Real> kProd(m_iRows, rkM.m_iRows);
    for (int iRow = 0; iRow < kProd.m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < kProd.m_iCols; iCol++)
        {
            for (int iMid = 0; iMid < m_iCols; iMid++)
            {
                kProd.m_aafEntry[iRow][iCol] += m_aafEntry[iRow][iMid] *
                                                rkM.m_aafEntry[iCol][iRow];
            }
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
GVector<Real> GMatrix<Real>::operator*(const GVector<Real> &rkV) const
{
    assert(rkV.GetSize() == m_iCols);
    GVector<Real> kProd(m_iRows);
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
            kProd[iRow] += m_aafEntry[iRow][iCol] * rkV[iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
GVector<Real> Wml::operator*(const GVector<Real> &rkV,
                             const GMatrix<Real> &rkM)
{
    assert(rkV.GetSize() == rkM.GetRows());
    GVector<Real> kProd(rkM.GetColumns());
    Real *afPEntry = kProd;
    for (int iCol = 0; iCol < rkM.GetColumns(); iCol++)
    {
        for (int iRow = 0; iRow < rkM.GetRows(); iRow++)
            afPEntry[iCol] += rkV[iRow] * rkM[iRow][iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
Real GMatrix<Real>::QForm(const GVector<Real> &rkU, const GVector<Real> &rkV)
    const
{
    assert(rkU.GetSize() == m_iRows && rkV.GetSize() == m_iCols);
    return rkU.Dot((*this) * rkV);
}
//----------------------------------------------------------------------------
