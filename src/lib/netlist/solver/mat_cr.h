// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * mat_cr.h
 *
 * Compressed row format matrices
 *
 */

#ifndef MAT_CR_H_
#define MAT_CR_H_

#include <algorithm>
#include "plib/pconfig.h"

template<int storage_N>
struct mat_cr_t
{
	unsigned nz_num;
	unsigned ia[storage_N + 1];
	unsigned ja[storage_N * storage_N];
	unsigned diag[storage_N];       /* n */

	template<typename T>
	void mult_vec(const T * RESTRICT A, const T * RESTRICT x, T * RESTRICT res)
	{
		/*
		 * res = A * x
		 */

		unsigned i = 0;
		unsigned k = 0;
		const unsigned oe = nz_num;

		while (k < oe)
		{
			T tmp = 0.0;
			const unsigned e = ia[i+1];
			for (; k < e; k++)
				tmp += A[k] * x[ja[k]];
			res[i++] = tmp;
		}
	}

	template<typename T>
	void incomplete_LU_factorization(const T * RESTRICT A, T * RESTRICT LU)
	{
		/*
		 * incomplete LU Factorization according to http://de.wikipedia.org/wiki/ILU-Zerlegung
		 *
		 * Result is stored in matrix LU
		 *
		 */

		const unsigned lnz = nz_num;

		for (unsigned k = 0; k < lnz; k++)
			LU[k] = A[k];

		for (unsigned i = 1; ia[i] < lnz; i++) // row i
		{
			const unsigned iai1 = ia[i + 1];
			const unsigned pke = diag[i];
			for (unsigned pk = ia[i]; pk < pke; pk++) // all columns left of diag in row i
			{
				// pk == (i, k)
				const unsigned k = ja[pk];
				const unsigned iak1 = ia[k + 1];
				const T LUpk = LU[pk] = LU[pk] / LU[diag[k]];

				unsigned pt = ia[k];

				for (unsigned pj = pk + 1; pj < iai1; pj++)  // pj = (i, j)
				{
					// we can assume that within a row ja increases continuously */
					const unsigned ej = ja[pj];
					while (ja[pt] < ej && pt < iak1)
						pt++;
					if (pt < iak1 && ja[pt] == ej)
						LU[pj] = LU[pj] - LUpk * LU[pt];
				}
			}
		}
	}

	template<typename T>
	void solveLUx (const T * RESTRICT LU, T * RESTRICT r)
	{
		/*
		 * Solve a linear equation Ax = r
		 * where
		 *      A = L*U
		 *
		 *      L unit lower triangular
		 *      U upper triangular
		 *
		 * ==> LUx = r
		 *
		 * ==> Ux = L?????r = w
		 *
		 * ==> r = Lw
		 *
		 * This can be solved for w using backwards elimination in L.
		 *
		 * Now Ux = w
		 *
		 * This can be solved for x using backwards elimination in U.
		 *
		 */

		unsigned i;

		for (i = 1; ia[i] < nz_num; i++ )
		{
			T tmp = 0.0;
			const unsigned j1 = ia[i];
			const unsigned j2 = diag[i];

			for (unsigned j = j1; j < j2; j++ )
				tmp +=  LU[j] * r[ja[j]];

			r[i] -= tmp;
		}
		// i now is equal to n;
		for (; 0 < i; i-- )
		{
			const unsigned im1 = i - 1;
			T tmp = 0.0;
			const unsigned j1 = diag[im1] + 1;
			const unsigned j2 = ia[im1+1];
			for (unsigned j = j1; j < j2; j++ )
				tmp += LU[j] * r[ja[j]];
			r[im1] = (r[im1] - tmp) / LU[diag[im1]];
		}
	}

};

#endif /* MAT_CR_H_ */
