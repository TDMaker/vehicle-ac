
// rdmat_mp get_M_mp(pairing_t *pairing)
// {
//     rdmat_f M_sp = gen_M();
//     rdmat_mp M_mp = make_rdmat_mp(M_sp.rows, M_sp.cols, pairing);
//     for (int i = 0; i < M_sp.rows; i++)
//     {
//         for (int j = 0; j < M_sp.cols; j++)
//         {
//             element_set_si(M_mp.elem[i * M_sp.cols + j], M_sp.elem[i * M_sp.cols + j]);
//         }
//     }
//     return M_mp;
// }


// rdmat_f gen_lambda(element_t* , rdmat_f M)
// {
//     int rho_data[] = {2, 3, 1, 4};
//     rdmat_f rho = {.elem = rho_data, .cols = 1, .rows = 4};

//     return mat_mul(M, rho);
// }

// rdmat_mp mat_mul_mp(rdmat_mp a, rdmat_mp b, pairing_t *pairing)
// {

//     rdmat_mp c = make_rdmat_mp(a.rows, b.cols, pairing);
//     element_t prod;
//     element_init_Zr(prod, *pairing);
//     for (int i = 0; i < a.rows; i++)
//     {
//         for (int j = 0; j < b.cols; j++)
//         {
//             element_set0(c.elem[i * b.cols + j]);
//             for (int k = 0; k < a.cols; k++)
//             {
//                 element_mul(prod, c.elem[i * a.cols + k], b.elem[k * b.cols + j]);
//                 element_add(c.elem[i * c.cols + j], c.elem[i * c.cols + j], prod);
//             }
//         }
//     }
//     element_clear(prod);
//     return c;
// }