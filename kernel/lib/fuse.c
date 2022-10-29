#include "fuse.h"
#include <types.h>
#include <drivers/serial.h>
#include <thirdparty/string/string.h>
 int add_opt_common(char **opts, const char *opt, int esc)
{
	unsigned oldlen = *opts ? strlen(*opts) : 0;
	char *d = realloc(*opts, oldlen + 1 + strlen(opt) * 2 + 1);

	if (!d)
		return alloc_failed();

	*opts = d;
	if (oldlen) {
		d += oldlen;
		*d++ = ',';
	}

	for (; *opt; opt++) {
		if (esc && (*opt == ',' || *opt == '\\'))
			*d++ = '\\';
		*d++ = *opt;
	}
	*d = '\0';

	return 0;
}
 int add_opt(struct fuse_opt_context *ctx, const char *opt)
{
	return add_opt_common(&ctx->opts, opt, 1);
}
 int process_gopt(struct fuse_opt_context *ctx, const char *arg, int iso)
{
	unsigned sep;
	const struct fuse_opt *opt = find_opt(ctx->opt, arg, &sep);
	if (opt) {
		for (; opt; opt = find_opt(opt + 1, arg, &sep)) {
			int res;
			if (sep && opt->templ[sep] == ' ' && !arg[sep])
				res = process_opt_sep_arg(ctx, opt, sep, arg,
							  iso);
			else
				res = process_opt(ctx, opt, sep, arg, iso);
			if (res == -1)
				return -1;
		}
		return 0;
	} else
		return call_proc(ctx, arg, FUSE_OPT_KEY_OPT, iso);
}
 int process_real_option_group(struct fuse_opt_context *ctx, char *opts)
{
	char *s = opts;
	char *d = s;
	int end = 0;

	while (!end) {
		if (*s == '\0')
			end = 1;
		if (*s == ',' || end) {
			int res;

			*d = '\0';
			res = process_gopt(ctx, opts, 1);
			if (res == -1)
				return -1;
			d = opts;
		} else {
			if (s[0] == '\\' && s[1] != '\0') {
				s++;
				if (s[0] >= '0' && s[0] <= '3' &&
				    s[1] >= '0' && s[1] <= '7' &&
				    s[2] >= '0' && s[2] <= '7') {
					*d++ = (s[0] - '0') * 0100 +
						(s[1] - '0') * 0010 +
						(s[2] - '0');
					s += 2;
				} else {
					*d++ = *s;
				}
			} else {
				*d++ = *s;
			}
		}
		s++;
	}

	return 0;
}

 int process_option_group(struct fuse_opt_context *ctx, const char *opts)
{
	int res;
	char *copy = strdup(opts);

	if (!copy) {
		fuse_log(0, "fuse: memory allocation failed\n");
		return -1;
	}
	res = process_real_option_group(ctx, copy);
	free(copy);
	return res;
}
 int call_proc(struct fuse_opt_context *ctx, const char *arg, int key,
		     int iso)
{
	if (key == FUSE_OPT_KEY_DISCARD)
		return 0;

	if (key != FUSE_OPT_KEY_KEEP && ctx->proc) {
		int res = ctx->proc(ctx->data, arg, key, &ctx->outargs);
		if (res == -1 || !res)
			return res;
	}
	if (iso)
		return add_opt(ctx, arg);
	else
		return add_arg(ctx, arg);
}

 int process_one(struct fuse_opt_context *ctx, const char *arg)
{
	if (ctx->nonopt || arg[0] != '-')
		return call_proc(ctx, arg, FUSE_OPT_KEY_NONOPT, 0);
	else if (arg[1] == 'o') {
		if (arg[2])
			return process_option_group(ctx, arg + 2);
		else {
			if (next_arg(ctx, arg) == -1)
				return -1;

			return process_option_group(ctx,
						    ctx->argv[ctx->argctr]);
		}
	} else if (arg[1] == '-' && !arg[2]) {
		if (add_arg(ctx, arg) == -1)
			return -1;
		ctx->nonopt = ctx->outargs.argc;
		return 0;
	} else
		return process_gopt(ctx, arg, 0);
}

 int alloc_failed(void)
{
	serial_printf("fuse: memory allocation failed\n");
	return -1;
}
 int fuse_opt_add_arg(struct fuse_args *args, const char *arg)
{
	char **newargv;
	char *newarg;

	// assert(!args->argv || args->allocated);
    if ((!args->argv || args->allocated)==0){
        return;
    }

	newarg = strdup(arg);
	if (!newarg)
		return alloc_failed();

	newargv = realloc(args->argv, (args->argc + 2) * sizeof(char *));
	if (!newargv) {
		free(newarg);
		return alloc_failed();
	}

	args->argv = newargv;
	args->allocated = 1;
	args->argv[args->argc++] = newarg;
	args->argv[args->argc] = NULL;
	return 0;
}
 int add_arg(struct fuse_opt_context *ctx, const char *arg)
{
	return fuse_opt_add_arg(&ctx->outargs, arg);
}
 int opt_parse(struct fuse_opt_context* ctx) {
    if (ctx->argc) {
        if (add_arg(ctx, ctx->argv[0]) == -1)
            return -1;
    }

    for (ctx->argctr = 1; ctx->argctr < ctx->argc; ctx->argctr++)
        if (process_one(ctx, ctx->argv[ctx->argctr]) == -1)
            return -1;

    if (ctx->opts) {
        if (fuse_opt_insert_arg(&ctx->outargs, 1, "-o") == -1 ||
            fuse_opt_insert_arg(&ctx->outargs, 2, ctx->opts) == -1)
            return -1;
    }

    /* If option separator ("--") is the last argument, remove it */
    if (ctx->nonopt && ctx->nonopt == ctx->outargs.argc &&
        strcmp(ctx->outargs.argv[ctx->outargs.argc - 1], "--") == 0) {
        free(ctx->outargs.argv[ctx->outargs.argc - 1]);
        ctx->outargs.argv[--ctx->outargs.argc] = NULL;
    }

    return 0;
}

 int fuse_opt_parse(struct fuse_args* args, void* data, const struct fuse_opt opts[],
                   fuse_opt_proc_t proc) {
    int res;
    struct fuse_opt_context ctx = {
        .data = data,
        .opt = opts,
        .proc = proc,
    };

    if (!args || !args->argv || !args->argc)
        return 0;

    ctx.argc = args->argc;
    ctx.argv = args->argv;

    res = opt_parse(&ctx);
    if (res != -1) {
        struct fuse_args tmp = *args;
        *args = ctx.outargs;
        ctx.outargs = tmp;
    }
    free(ctx.opts);
    fuse_opt_free_args(&ctx.outargs);
    return res;
}
