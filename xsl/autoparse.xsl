<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:mddl="http://www.mddl.org/mddl/3.0-beta"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  version="1.0">

  <xsl:output method="text"/>
  <xsl:param name="hdr"/>

  <xsl:include href="autospec-common.xsl"/>

  <xsl:template match="spec">
    <xsl:text>/* AUTO-GENERATED, DO NOT MODIFY */&#0010;&#0010;</xsl:text>

    <!-- decl -->
    <xsl:apply-templates mode="decl"/>

    <xsl:text>&#0010;&#0010;</xsl:text>

    <!-- def -->
    <xsl:apply-templates mode="def"/>

    <!-- global attrs, bos and eos -->
    <xsl:text>static void&#0010;parse_bo</xsl:text>
    <xsl:text>(mddl_ctx_t ctx, mddl_tid_t tid, const char **attrs)</xsl:text>
    <xsl:text>&#0010;{&#0010;</xsl:text>
    <xsl:text>&#0009;switch (get_state_otype(ctx)) {&#0010;</xsl:text>
    <xsl:apply-templates mode="glob_bo"/>
    <xsl:text>&#0009;}&#0010;</xsl:text>
    <xsl:text>&#0009;return;&#0010;</xsl:text>
    <xsl:text>}&#0010;</xsl:text>

    <xsl:text>static void&#0010;parse_eo</xsl:text>
    <xsl:text>(mddl_ctx_t ctx, mddl_tid_t tid)</xsl:text>
    <xsl:text>&#0010;{&#0010;</xsl:text>
    <xsl:text>&#0009;switch (get_state_otype(ctx)) {&#0010;</xsl:text>
    <xsl:apply-templates mode="glob_eo"/>
    <xsl:text>&#0009;}&#0010;</xsl:text>
    <xsl:text>&#0009;return;&#0010;</xsl:text>
    <xsl:text>}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="decl">
    <xsl:text>static void parse_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>_attrs(mddl_ctx_t, </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text>*, const char **attrs);&#0010;</xsl:text>

    <xsl:text>static void parse_bo_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_ctx_t, mddl_tid_t, const char **attrs);&#0010;</xsl:text>

    <xsl:text>static void parse_eo_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_ctx_t ctx, mddl_tid_t tid);&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="def">
    <!-- attrs -->
    <xsl:text>static void&#0010;parse_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>_attrs(mddl_ctx_t ctx, </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> *o, const char **attrs)</xsl:text>
    <xsl:text>
{
	if (attrs == NULL) {
		return;
	}
	for (const char **a = attrs; *a; a += 2) {
		mddl_aid_t aid = sax_aid_from_attr(*a);
		const char *v = *(a + 1);

		switch (aid) {
</xsl:text>

    <!-- all the rest -->
    <xsl:apply-templates mode="attr">
      <xsl:with-param name="super" select="@slot"/>
    </xsl:apply-templates>

    <xsl:text>
		case MDDL_ATTR_UNK:
			break;
		default:
			fprintf(stderr, "aid %u illegal inside </xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>\n", aid);
			break;
		}
	}
	return;
}
</xsl:text>

    <!-- bo tag -->
    <xsl:text>static void&#0010;parse_bo_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_ctx_t ctx, mddl_tid_t tid, const char **attrs)</xsl:text>
    <xsl:text>&#0010;{&#0010;</xsl:text>

    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> *o = get_state_object(ctx);&#0010;</xsl:text>

    <xsl:text>&#0009;switch (tid) {&#0010;</xsl:text>

    <!-- all the rest -->
    <xsl:apply-templates mode="bo">
      <xsl:with-param name="super" select="@slot"/>
    </xsl:apply-templates>

    <xsl:text>&#0009;default:&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;fprintf(stderr, "tid %u illegal inside </xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>\n", tid);&#0010;</xsl:text>

    <xsl:text>&#0009;}&#0010;</xsl:text>

    <xsl:text>&#0009;return;&#0010;</xsl:text>
    <xsl:text>}&#0010;&#0010;</xsl:text>

    <!-- eo tag -->
    <xsl:text>static void&#0010;parse_eo_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(mddl_ctx_t ctx, mddl_tid_t tid)</xsl:text>
    <xsl:text>&#0010;{&#0010;</xsl:text>

    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> *o = get_state_object(ctx);&#0010;</xsl:text>

    <xsl:text>&#0009;switch (tid) {&#0010;</xsl:text>

    <!-- all the rest -->
    <xsl:apply-templates mode="eo">
      <xsl:with-param name="super" select="@slot"/>
    </xsl:apply-templates>

    <xsl:text>&#0009;default:&#0010;</xsl:text>
    <!-- fix me, we need a better test than this -->
    <xsl:if test="name(*[1]) = 'slot' and not(*[1]/@attr)">
      <xsl:text>&#0009;&#0009;if (</xsl:text>
      <xsl:text>__</xsl:text>
      <xsl:value-of select="./*[1]/@class"/>
      <xsl:text>_null_p(o-&gt;</xsl:text>
      <xsl:value-of select="./*[1]/@slot"/>
      <xsl:text>)) {&#0010;</xsl:text>
      <xsl:text>&#0009;&#0009;&#00009;</xsl:text>
      <xsl:value-of select="./*[1]/@type"/>
      <xsl:text> p = stuff_buf_to_</xsl:text>
      <xsl:value-of select="./*[1]/@class"/>
      <xsl:text>(ctx);&#0010;</xsl:text>
      <xsl:text>&#0009;&#0009;&#00009;mddl_</xsl:text>
      <xsl:value-of select="@slot"/>
      <xsl:text>_set_</xsl:text>
      <xsl:value-of select="./*[1]/@slot"/>
      <xsl:text>(o, p);&#0010;</xsl:text>
      <xsl:text>&#0009;&#0009;}&#0010;</xsl:text>
    </xsl:if>

    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>
    <xsl:text>&#0009;}&#0010;</xsl:text>

    <xsl:text>&#0009;return;&#0010;</xsl:text>
    <xsl:text>}&#0010;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot[@attr]" mode="attr">
    <xsl:param name="super"/>

    <xsl:text>&#0009;&#0009;case MDDL_ATTR_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>: {&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;&#0009;</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> p = attr_to_</xsl:text>
    <xsl:value-of select="@class"/>
    <xsl:text>(v);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;&#0009;mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_set_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(o, p);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;&#0009;break;&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult = '*']" mode="bo">
    <xsl:param name="super"/>

    <xsl:text>&#0009;case MDDL_TAG_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>: {&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;void *p = mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_add_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(o);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;parse_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>_attrs(ctx, p, attrs);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;push_state(ctx, tid, p);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>

    <xsl:text>&#0009;}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult != '*']" mode="bo">
    <xsl:param name="super"/>

    <xsl:text>&#0009;case MDDL_TAG_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>: {&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;void *p = o-&gt;</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>;&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;push_state(ctx, tid, p);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>
    <xsl:text>&#0009;}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot[not(@attr)]" mode="bo">
    <xsl:param name="super"/>

    <xsl:text>&#0009;case MDDL_TAG_</xsl:text>
    <xsl:value-of select="@class"/>
    <xsl:text>:&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;stuff_buf_reset(ctx);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult = '*']" mode="eo">
    <xsl:param name="super"/>

    <xsl:text>&#0009;case MDDL_TAG_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>:&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct[@mult != '*']" mode="eo">
    <xsl:param name="super"/>

    <xsl:text>&#0009;case MDDL_TAG_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>: {&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>
    <xsl:text>&#0009;}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot[not(@attr)]" mode="eo">
    <xsl:param name="super"/>

    <xsl:text>&#0009;case MDDL_TAG_</xsl:text>
    <xsl:value-of select="@class"/>
    <xsl:text>: {&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;</xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:text> p = stuff_buf_to_</xsl:text>
    <xsl:value-of select="@class"/>
    <xsl:text>(ctx);&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;mddl_</xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text>_set_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(o, p);&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>
    <xsl:text>&#0009;}&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="glob_bo">
    <xsl:text>&#0009;case MDDL_TAG_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>:&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;parse_bo_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(ctx, tid, attrs);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="glob_eo">
    <xsl:text>&#0009;case MDDL_TAG_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>:&#0010;</xsl:text>

    <xsl:text>&#0009;&#0009;parse_eo_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>(ctx, tid);&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="glob_attr">
    <xsl:text>&#0009;case MDDL_TAG_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>:&#0010;</xsl:text>

    <xsl:text>		for (const char **a = attrs; *a; a += 2) {
			mddl_aid_t aid = sax_aid_from_attr(*a);
			parse_</xsl:text>
    <xsl:value-of select="@slot"/>
    <xsl:text>_attr(ctx, aid, *(a + 1));&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;}&#0010;</xsl:text>
    <xsl:text>&#0009;&#0009;break;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="decl"/>
  <xsl:template match="text()" mode="def"/>
  <xsl:template match="text()" mode="attr"/>
  <xsl:template match="text()" mode="bo"/>
  <xsl:template match="text()" mode="eo"/>
  <xsl:template match="text()" mode="glob_bo"/>
  <xsl:template match="text()" mode="glob_eo"/>
  <xsl:template match="text()" mode="glob_attr"/>
  <xsl:template match="*"/>
  <xsl:template match="*" mode="decl"/>
  <xsl:template match="*" mode="def"/>
  <xsl:template match="*" mode="attr"/>
  <xsl:template match="*" mode="bo"/>
  <xsl:template match="*" mode="eo"/>
  <xsl:template match="*" mode="glob_bo"/>
  <xsl:template match="*" mode="glob_eo"/>
  <xsl:template match="*" mode="glob_attr"/>

</xsl:stylesheet>
