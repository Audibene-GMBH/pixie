import { DOMAIN_NAME, SEGMENT_UI_WRITE_KEY } from 'containers/constants';
import { format } from 'date-fns';

const { STABLE_BUILD_NUMBER } = process.env;
const { STABLE_BUILD_SCM_REVISION } = process.env;
const { STABLE_BUILD_SCM_STATUS } = process.env;
const { BUILD_TIMESTAMP } = process.env;

const timestampSec = Number.parseInt(BUILD_TIMESTAMP, 10);
const date = Number.isNaN(timestampSec) ? new Date() : new Date(timestampSec * 1000);
const dateStr = format(date, 'YYYY.MM.DD.hh.mm');
const parts = [];
if (typeof STABLE_BUILD_SCM_REVISION === 'string') {
  parts.push(STABLE_BUILD_SCM_REVISION.substr(0, 7));
}
if (STABLE_BUILD_SCM_STATUS) {
  parts.push(STABLE_BUILD_SCM_STATUS);
}
parts.push(Number.isNaN(timestampSec) ? Math.floor(date.valueOf() / 1000) : timestampSec);
if (STABLE_BUILD_NUMBER) {
  parts.push(STABLE_BUILD_NUMBER);
}

export const PIXIE_CLOUD_VERSION = `${dateStr}+${parts.join('.')}`;

export function isDev(): boolean {
  return DOMAIN_NAME.startsWith('dev');
}

export function isStaging(): boolean {
  return DOMAIN_NAME.startsWith('staging');
}

export function isProd(): boolean {
  return !isDev() && !isStaging();
}

function isValidSegmentKey(k) {
  // The TS compiler is really smart and is optmizing away the checks,
  // which is why this check is so convoluted...
  return k && !k.startsWith('__S');
}

export function isValidAnalytics(): boolean {
  return isValidSegmentKey(SEGMENT_UI_WRITE_KEY);
}
